// src/platform/window.cpp
#include <ecs/platform/window.h>
#include "osWindow.h"
#include <ecs/core/world.h>
#include <stdexcept>


namespace ecs::platform {

	// pollWindows and applyWindows go here.
	void applyWindows(World& w, float) {
		struct Pending { Entity e; SDL_Window* window; SDL_Renderer* renderer; };
		std::vector<Pending> created;
		w.each<WindowDesc>(exclude<OsWindow>, [&](Entity e, WindowDesc& windowDesc) {
			SDL_Window* window = SDL_CreateWindow(windowDesc.title.c_str(), windowDesc.w, windowDesc.h, NULL);
			if (!window) throw std::runtime_error(SDL_GetError());
			SDL_Renderer* renderer = SDL_CreateRenderer(window, "test");
			if (!renderer) throw std::runtime_error(SDL_GetError());
			w.add<OsWindow>(e, OsWindow(window, renderer));
			created.push_back(Pending{ e, window, renderer });
		});

		for (auto& p : created)
			w.add(p.e, OsWindow{ p.window, p.renderer });
	}

	void pollWindows() {

	}
}