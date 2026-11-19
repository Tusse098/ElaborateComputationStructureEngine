// include/ecs/debugging/debug.h
//
// Debugging helpers. Nothing here is needed to run the engine; it answers
// "what is this entity?" and "is the storage still consistent?" while bug
// hunting. Everything is read-only. Core never includes this file.
#pragma once

#include <string>
#include <ecs/core/entity.h>
#include <ecs/platform/window.h>


namespace ecs::platform::debug {

	// "entity(7, gen 1)", or "entity(null)".
	void printWindowDesc(const WindowDesc&);
}