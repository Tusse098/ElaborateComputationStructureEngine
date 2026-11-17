#pragma once
#include <SDL3/SDL.h>

namespace ecs::platform {
	struct osWindow {
		SDL_Window* window;
		SDL_Renderer* renderer;
	};
}