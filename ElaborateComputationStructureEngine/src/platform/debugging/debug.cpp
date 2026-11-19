// src/debugging/debug.cpp
#include <ecs/platform/debugging/debug.h>
#include <ecs/core/world.h>

#include <cstdio>
#include <ecs/platform/window.h>

namespace ecs::platform::debug {

    void printWindowDesc(const ecs::platform::WindowDesc& d) {
        std::printf("WindowDesc \"%s\"  pos (%d, %d)  size %dx%d  %s%s%s%s\n",
            d.title.c_str(), d.x, d.y, d.w, d.h,
            d.borderless ? "borderless " : "",
            d.userMovable ? "movable " : "",
            d.userResizable ? "resizable " : "",
            d.visible ? "visible" : "hidden");
    }

}