#pragma once
#include <SDL3/SDL.h>

namespace ecs::platform {
	struct OsWindow {
		SDL_Window* window;
		SDL_Renderer* renderer;

		OsWindow() = default;
		OsWindow(SDL_Window* w, SDL_Renderer* r) : window(w), renderer(r) { }
		~OsWindow() {
			if (renderer) SDL_DestroyRenderer(renderer);
			if (window) SDL_DestroyWindow(window);
		}

		OsWindow(const OsWindow&) = delete;
		OsWindow& operator=(const OsWindow&) = delete;
		OsWindow(OsWindow&& o) noexcept : window(o.window), renderer(o.renderer) { o.window = nullptr; o.renderer = nullptr; }
		OsWindow& operator=(OsWindow&& o) noexcept {
			if (this != &o) {
				if (renderer) SDL_DestroyRenderer(renderer);
				if (window) SDL_DestroyWindow(window);
				window = o.window;
				renderer = o.renderer;
				o.window = nullptr; o.renderer = nullptr;
			}
			return *this;
		}
	};
}