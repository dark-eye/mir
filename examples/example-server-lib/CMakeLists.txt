find_package(PIL REQUIRED)
pkg_check_modules(FREETYPE freetype2 REQUIRED)

add_library(example-shell-lib STATIC
    tiling_window_manager.cpp   tiling_window_manager.h
    floating_window_manager.cpp floating_window_manager.h
    decoration_provider.cpp     decoration_provider.h
    wallpaper_config.cpp        wallpaper_config.h
        splash_session.h
    sw_splash.cpp               sw_splash.h
    wayland_helpers.cpp         wayland_helpers.h
)

target_include_directories(example-shell-lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include/client
    PRIVATE
        ${FREETYPE_INCLUDE_DIRS}
)
target_link_libraries(example-shell-lib PUBLIC miral PRIVATE ${WAYLAND_CLIENT_LIBRARIES} ${FREETYPE_LIBRARIES})
