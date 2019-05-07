/*
 * AUTOGENERATED - DO NOT EDIT
 *
 * This file is generated from xdg-output-unstable-v1.xml
 * To regenerate, run the “refresh-wayland-wrapper” target.
 */

#ifndef MIR_FRONTEND_WAYLAND_XDG_OUTPUT_UNSTABLE_V1_XML_WRAPPER
#define MIR_FRONTEND_WAYLAND_XDG_OUTPUT_UNSTABLE_V1_XML_WRAPPER

#include <experimental/optional>

#include "mir/fd.h"
#include <wayland-server-core.h>

namespace mir
{
namespace wayland
{

class XdgOutputManagerV1
{
public:
    static char const constexpr* interface_name = "zxdg_output_manager_v1";
    static int const interface_version = 2;

    static XdgOutputManagerV1* from(struct wl_resource*);

    XdgOutputManagerV1(struct wl_display* display, uint32_t max_version);
    virtual ~XdgOutputManagerV1();

    void destroy_wayland_object(struct wl_resource* resource) const;

    struct wl_global* const global;
    uint32_t const max_version;

    struct Thunks;

private:
    virtual void bind(struct wl_client* client, struct wl_resource* resource) { (void)client; (void)resource; }

    virtual void destroy(struct wl_client* client, struct wl_resource* resource) = 0;
    virtual void get_xdg_output(struct wl_client* client, struct wl_resource* resource, struct wl_resource* id, struct wl_resource* output) = 0;
};

class XdgOutputV1
{
public:
    static char const constexpr* interface_name = "zxdg_output_v1";
    static int const interface_version = 2;

    static XdgOutputV1* from(struct wl_resource*);

    XdgOutputV1(struct wl_resource* resource);
    virtual ~XdgOutputV1() = default;

    void send_logical_position_event(int32_t x, int32_t y) const;
    void send_logical_size_event(int32_t width, int32_t height) const;
    void send_done_event() const;
    bool version_supports_name();
    void send_name_event(std::string const& name) const;
    bool version_supports_description();
    void send_description_event(std::string const& description) const;

    void destroy_wayland_object() const;

    struct wl_client* const client;
    struct wl_resource* const resource;

    struct Opcode
    {
        static uint32_t const logical_position = 0;
        static uint32_t const logical_size = 1;
        static uint32_t const done = 2;
        static uint32_t const name = 3;
        static uint32_t const description = 4;
    };

    struct Thunks;

    static bool is_instance(wl_resource* resource);

private:
    virtual void destroy() = 0;
};

}
}

#endif // MIR_FRONTEND_WAYLAND_XDG_OUTPUT_UNSTABLE_V1_XML_WRAPPER
