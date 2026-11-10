// include/ecs/platform/window.h  — no SDL here
struct SDL_Window;                       // forward declaration: "exists, defined elsewhere"

namespace ecs::platform {
    class Window {
    public:
        Window(const char* title, int w, int h);
        ~Window();
        bool pollEvents();               // false when the user closed it
    private:
        SDL_Window* handle = nullptr;    // a pointer to an incomplete type is fine
    };
}