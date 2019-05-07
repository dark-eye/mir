/*
 * AUTOGENERATED - DO NOT EDIT
 *
 * This file is generated from wlr-layer-shell-unstable-v1.xml
 * To regenerate, run the “refresh-wayland-wrapper” target.
 */

#ifndef MIR_FRONTEND_WAYLAND_WLR_LAYER_SHELL_UNSTABLE_V1_XML_WRAPPER
#define MIR_FRONTEND_WAYLAND_WLR_LAYER_SHELL_UNSTABLE_V1_XML_WRAPPER

#include <experimental/optional>

#include "mir/fd.h"
#include <wayland-server-core.h>

namespace mir
{
namespace wayland
{

class LayerShellV1
{
public:
    static char const constexpr* interface_name = "zwlr_layer_shell_v1";
    static int const interface_version = 1;

    static LayerShellV1* from(struct wl_resource*);

    LayerShellV1(struct wl_display* display, uint32_t max_version);
    virtual ~LayerShellV1();

    void destroy_wayland_object(struct wl_resource* resource) const;

    struct wl_global* const global;
    uint32_t const max_version;

    struct Error
    {
        static uint32_t const role = 0;
        static uint32_t const invalid_layer = 1;
        static uint32_t const already_constructed = 2;
    };

    struct Layer
    {
        static uint32_t const background = 0;
        static uint32_t const bottom = 1;
        static uint32_t const top = 2;
        static uint32_t const overlay = 3;
    };

    struct Thunks;

private:
    virtual void bind(struct wl_client* client, struct wl_resource* resource) { (void)client; (void)resource; }

    virtual void get_layer_surface(struct wl_client* client, struct wl_resource* resource, struct wl_resource* id, struct wl_resource* surface, std::experimental::optional<struct wl_resource*> const& output, uint32_t layer, std::string const& namespace_) = 0;
};

class LayerSurfaceV1
{
public:
    static char const constexpr* interface_name = "zwlr_layer_surface_v1";
    static int const interface_version = 1;

    static LayerSurfaceV1* from(struct wl_resource*);

    LayerSurfaceV1(struct wl_resource* resource);
    virtual ~LayerSurfaceV1() = default;

    void send_configure_event(uint32_t serial, uint32_t width, uint32_t height) const;
    void send_closed_event() const;

    void destroy_wayland_object() const;

    struct wl_client* const client;
    struct wl_resource* const resource;

    struct Error
    {
        static uint32_t const invalid_surface_state = 0;
        static uint32_t const invalid_size = 1;
        static uint32_t const invalid_anchor = 2;
    };

    struct Anchor
    {
        static uint32_t const top = 1;
        static uint32_t const bottom = 2;
        static uint32_t const left = 4;
        static uint32_t const right = 8;
    };

    struct Opcode
    {
        static uint32_t const configure = 0;
        static uint32_t const closed = 1;
    };

    struct Thunks;

    static bool is_instance(wl_resource* resource);

private:
    virtual void set_size(uint32_t width, uint32_t height) = 0;
    virtual void set_anchor(uint32_t anchor) = 0;
    virtual void set_exclusive_zone(int32_t zone) = 0;
    virtual void set_margin(int32_t top, int32_t right, int32_t bottom, int32_t left) = 0;
    virtual void set_keyboard_interactivity(uint32_t keyboard_interactivity) = 0;
    virtual void get_popup(struct wl_resource* popup) = 0;
    virtual void ack_configure(uint32_t serial) = 0;
    virtual void destroy() = 0;
};

}
}

#endif // MIR_FRONTEND_WAYLAND_WLR_LAYER_SHELL_UNSTABLE_V1_XML_WRAPPER
