// src/platform/window.cpp
#include <ecs/platform/window.h>
#include "osWindow.h"
#include <ecs/core/world.h>
#include <ecs/core/scheduler.h>
#include <stdexcept>
#include <ecs/platform/input.h>

namespace ecs::platform {
	static Entity entityFor(SDL_WindowID id);
	static constexpr const char* kEntityProp = "ecs.entity";

	// pollWindows and applyWindows go here.
	void applyWindows(World& w, float) {
		struct Pending { Entity e; SDL_Window* window; SDL_Renderer* renderer; };
		std::vector<Pending> created;
		w.each<WindowDesc>(exclude<OsWindow>, [&](Entity e, WindowDesc& windowDesc) {
			SDL_Window* window = SDL_CreateWindow(windowDesc.title.c_str(), windowDesc.w, windowDesc.h, 0);
			if (!window) throw std::runtime_error(SDL_GetError());
			SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
			if (!renderer) throw std::runtime_error(SDL_GetError());
			created.push_back(Pending{ e, window, renderer });
			SDL_SetNumberProperty(SDL_GetWindowProperties(window), kEntityProp, e); // To store the entityId for O(1) lookup of the OsWindows
		});

		for (auto& p : created) {
			w.add(p.e, OsWindow{ p.window, p.renderer });
			//w.add(p.e, WindowState{});
		}
	}

	void pollWindows(World& w, float dt) {
		
		if (!w.hasResource<Input>()) w.setResource(Input{});
		SDL_Event event;

		Input& input = w.resource<Input>();

		for (bool& b : input.mousePressed)  b = false;
		for (bool& b : input.mouseReleased) b = false;

		while(SDL_PollEvent(&event)) {
			Entity entity = entityFor(event.motion.windowID);
			switch(event.type){
				case SDL_EVENT_MOUSE_BUTTON_DOWN: {
					int b = event.button.button - 1;
					if (b < 3) { input.mouseDown[b] = true;  input.mousePressed[b] = true; }
					break;
				}
				case SDL_EVENT_MOUSE_BUTTON_UP: {
					int b = event.button.button - 1;
					if (b < 3) { input.mouseDown[b] = false; input.mouseReleased[b] = true; }
					break;
				}
				case SDL_EVENT_MOUSE_MOTION:
					input.mouseX = event.motion.x; input.mouseY = event.motion.y;
					input.mouseWindow = entity;     // the property lookup
					break;
			}
		}
		//SDL_Window* win = SDL_GetWindowFromID(ev.window.windowID);          // SDL's own O(1) lookup
		//if (!win) continue;                                                 // destroyed; events still in flight
		

		//w.each<OsWindow>([&](Entity e, OsWindow) {

		//	});
	}
	// So we get all the events. Get the matching OsWindow using 

	void addWindowSystems(Scheduler& s) {
		s.add(applyWindows);
		s.add(pollWindows);
	}

	static Entity entityFor(SDL_WindowID id) {
		SDL_Window* win = SDL_GetWindowFromID(id);
		if (!win) return null;                                   // destroyed; late events are normal
		return static_cast<Entity>(
			SDL_GetNumberProperty(SDL_GetWindowProperties(win), kEntityProp, null));
	}
}