/*
 * Copyright © 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Alan Griffiths <alan@octopull.co.uk>
 */

#include "mir_test_framework/testing_server_configuration.h"

#include "mir/graphics/platform_ipc_package.h"
#include "mir/compositor/renderer.h"
#include "mir/compositor/renderer_factory.h"
#include "mir/graphics/buffer_basic.h"
#include "mir/graphics/buffer_properties.h"
#include "mir/graphics/buffer_ipc_packer.h"
#include "mir/graphics/graphic_buffer_allocator.h"
#include "mir/input/input_channel.h"
#include "mir/input/input_manager.h"
#include "mir/input/null_input_configuration.h"
#include "src/server/frontend/server_connection_implementations.h"

#include "mir_test_doubles/stub_buffer.h"
#include "mir_test_doubles/stub_surface_builder.h"
#include "mir_test_doubles/null_platform.h"
#include "mir_test_doubles/null_display.h"
#include "mir_test_doubles/stub_display_buffer.h"

#include <gtest/gtest.h>
#include <thread>

#include <boost/exception/errinfo_errno.hpp>
#include <boost/throw_exception.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace geom = mir::geometry;
namespace mc = mir::compositor;
namespace mg = mir::graphics;
namespace mi = mir::input;
namespace ms = mir::surfaces;
namespace mf = mir::frontend;
namespace mtf = mir_test_framework;
namespace mtd = mir::test::doubles;
namespace msh = mir::shell;

namespace
{
char const* dummy[] = {0};
int argc = 0;
char const** argv = dummy;

class StubFDBuffer : public mtd::StubBuffer
{
public:
    StubFDBuffer(mg::BufferProperties const& properties)
        : StubBuffer(properties),
          properties{properties}
    {
        fd = open("/dev/zero", O_RDONLY);
        if (fd < 0)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(
                    std::runtime_error("Failed to open dummy fd")) << boost::errinfo_errno(errno));
    }

    std::shared_ptr<MirNativeBuffer> native_buffer_handle() const override
    {
#ifndef ANDROID
        auto native_buffer = std::make_shared<MirNativeBuffer>();
        native_buffer->data_items = 1;
        native_buffer->data[0] = 0xDEADBEEF;
        native_buffer->fd_items = 1;
        native_buffer->fd[0] = fd;

        native_buffer->flags = 0;
        if (properties.size.width.as_int() >= 800 &&
            properties.size.height.as_int() >= 600 &&
            properties.usage == mg::BufferUsage::hardware)
        {
            native_buffer->flags |= mir_buffer_flag_can_scanout;
        }
        return native_buffer;
#else
        return std::shared_ptr<MirNativeBuffer>();
#endif
    }

    ~StubFDBuffer() noexcept
    {
        close(fd);
    }
private:
    int fd;
    const mg::BufferProperties properties;
};

class StubGraphicBufferAllocator : public mg::GraphicBufferAllocator
{
 public:
    std::shared_ptr<mg::Buffer> alloc_buffer(mg::BufferProperties const& properties)
    {
        return std::unique_ptr<mg::Buffer>(new StubFDBuffer(properties));
    }

    std::vector<geom::PixelFormat> supported_pixel_formats()
    {
        return std::vector<geom::PixelFormat>();
    }
};

class StubDisplayConfiguration : public mtd::NullDisplayConfiguration
{
public:
    StubDisplayConfiguration(geom::Rectangle& rect)
         : modes{mg::DisplayConfigurationMode{rect.size, 1.0f}}
    {
    }

    void for_each_output(std::function<void(mg::DisplayConfigurationOutput const&)> f) const override
    {
        mg::DisplayConfigurationOutput dummy_output_config{
            mg::DisplayConfigurationOutputId{1},
            mg::DisplayConfigurationCardId{0},
            mg::DisplayConfigurationOutputType::vga,
            std::vector<geom::PixelFormat>{geom::PixelFormat::abgr_8888},
            modes, 0, geom::Size{}, true, true, geom::Point{0,0}, 0, 0, mir_power_mode_on};

        f(dummy_output_config);
    }
private:
    std::vector<mg::DisplayConfigurationMode> modes;
};

class StubDisplay : public mtd::NullDisplay
{
public:
    StubDisplay()
        : rect{geom::Point{0,0}, geom::Size{1600,1600}},
          display_buffer(rect)
    {
    }

    void for_each_display_buffer(std::function<void(mg::DisplayBuffer&)> const& f) override
    {
        f(display_buffer);
    }
 
    std::shared_ptr<mg::DisplayConfiguration> configuration() override
    {
        return std::make_shared<StubDisplayConfiguration>(rect);
    }

private:
    geom::Rectangle rect;
    mtd::StubDisplayBuffer display_buffer;
};

class StubGraphicPlatform : public mtd::NullPlatform
{
    std::shared_ptr<mg::GraphicBufferAllocator> create_buffer_allocator(
        const std::shared_ptr<mg::BufferInitializer>& /*buffer_initializer*/) override
    {
        return std::make_shared<StubGraphicBufferAllocator>();
    }

    void fill_ipc_package(std::shared_ptr<mg::BufferIPCPacker> const& packer,
                          std::shared_ptr<mg::Buffer> const& buffer) const override
    {
#ifndef ANDROID
        auto native_handle = buffer->native_buffer_handle();
        for(auto i=0; i<native_handle->data_items; i++)
        {
            packer->pack_data(native_handle->data[i]);
        }
        for(auto i=0; i<native_handle->fd_items; i++)
        {
            packer->pack_fd(native_handle->fd[i]);
        }

        packer->pack_stride(buffer->stride());
        packer->pack_flags(native_handle->flags);
#else
        (void)packer;
        (void)buffer;
#endif
    }

    std::shared_ptr<mg::Display> create_display(
        std::shared_ptr<mg::DisplayConfigurationPolicy> const&) override
    {
        return std::make_shared<StubDisplay>();
    }
};

class StubRenderer : public mc::Renderer
{
public:
    virtual void render(std::function<void(std::shared_ptr<void> const&)>,
                        mc::CompositingCriteria const&, ms::BufferStream& stream)
    {
        // Need to acquire the texture to cycle buffers
        stream.lock_compositor_buffer(++frameno);
    }

    void clear(unsigned long) override {}

    unsigned long frameno = 0;
};

class StubRendererFactory : public mc::RendererFactory
{
public:
    std::unique_ptr<mc::Renderer> create_renderer_for(geom::Rectangle const&)
    {
        return std::unique_ptr<StubRenderer>(new StubRenderer());
    }
};

struct StubInputChannel : public mi::InputChannel
{
    int client_fd() const
    {
        return 0;
    }

    int server_fd() const
    {
        return 0;
    }
};

class StubInputManager : public mi::InputManager
{
  public:
    void start() {}
    void stop() {}

    std::shared_ptr<mi::InputChannel> make_input_channel()
    {
        return std::make_shared<StubInputChannel>();
    }
};

}

mtf::TestingServerConfiguration::TestingServerConfiguration() :
    DefaultServerConfiguration(::argc, ::argv)
{
    namespace po = boost::program_options;

    add_options()
        ("tests-use-real-graphics", po::value<bool>(), "Use real graphics in tests. [bool:default=false]")
        ("tests-use-real-input", po::value<bool>(), "Use real input in tests. [bool:default=false]");
}

std::shared_ptr<mi::InputConfiguration> mtf::TestingServerConfiguration::the_input_configuration()
{
    auto options = the_options();

    if (options->get("tests-use-real-input", false))
        return DefaultServerConfiguration::the_input_configuration();
    else
        return std::make_shared<mi::NullInputConfiguration>();
}

std::shared_ptr<mg::Platform> mtf::TestingServerConfiguration::the_graphics_platform()
{
    if (!graphics_platform)
    {
        graphics_platform = std::make_shared<StubGraphicPlatform>();
    }

    return graphics_platform;
}

std::shared_ptr<mc::RendererFactory> mtf::TestingServerConfiguration::the_renderer_factory()
{
    auto options = the_options();

    if (options->get("tests-use-real-graphics", false))
        return DefaultServerConfiguration::the_renderer_factory();
    else
        return renderer_factory(
            [&]()
            {
                return std::make_shared<StubRendererFactory>();
            });
}

void mtf::TestingServerConfiguration::exec()
{
}

void mtf::TestingServerConfiguration::on_exit()
{
}

std::shared_ptr<mf::SocketConnection> mtf::TestingServerConfiguration::the_socket()
{
    return socket([this]
    {
        return std::make_shared<mf::FileSocketConnection>(test_socket_file());
    });
}


std::string const& mtf::test_socket_file()
{
    static const std::string socket_file{"./mir_socket_test"};
    return socket_file;
}


int main(int argc, char** argv)
{
    ::argc = std::remove_if(
        argv,
        argv+argc,
        [](char const* arg) { return !strncmp(arg, "--gtest_", 8); }) - argv;
    ::argv = const_cast<char const**>(argv);

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
