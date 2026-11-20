#pragma once
#include <string>
#include <ecs/fwd.h>

namespace ecs::platform {

    struct WindowDesc {      // description written by the game
        std::string title;
        int x = 100, y = 100, w = 640, h = 480;
        bool borderless = false;
        bool userMovable = false;
        bool userResizable = false;
        bool visible = true;
    };

    struct WindowState {     // state written by pollWindows
        int  x = 0, y = 0, w = 0, h = 0;
        bool focused = false;
        bool moved = false, resized = false;
        bool closeRequested = false;
    };

    void addWindowSystems(Scheduler& s);

}