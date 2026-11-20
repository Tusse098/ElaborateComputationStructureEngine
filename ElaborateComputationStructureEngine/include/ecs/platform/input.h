// include/ecs/platform/input.h 

namespace ecs::platform {
    enum MouseButton { Left = 0, Middle = 1, Right = 2 };

    struct Input {                       // written by pollWindows, read by everyone
        float mouseX = 0, mouseY = 0;    // in which window? — see below
        bool  mouseDown[3] = {};      // held right now
        bool  mousePressed[3] = {};      // went down this tick
        bool  mouseReleased[3] = {};      // went up this tick
        Entity mouseWindow = null;       // which window entity the mouse is over
    };
}