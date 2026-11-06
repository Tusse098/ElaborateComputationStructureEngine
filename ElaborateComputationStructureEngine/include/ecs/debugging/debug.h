// include/ecs/debugging/debug.h
//
// Debugging helpers. Nothing here is needed to run the engine; it answers
// "what is this entity?" and "is the storage still consistent?" while bug
// hunting. Everything is read-only. Core never includes this file.
#pragma once

#include <string>
#include <ecs/core/entity.h>

namespace ecs {
	class World;
}

namespace ecs::debug {

	// "entity(7, gen 1)", or "entity(null)".
	std::string toString(Entity e);

	// One entity: handle, alive/dead, and the type name of every component it has.
	std::string describe(const World& w, Entity e);

	// Every pool with its size, in typeId order.
	std::string summary(const World& w);

	// Checks the sparse-set invariants of every pool. Returns true if all hold;
	// otherwise prints which pool broke and returns false. Cheap enough to call
	// after every flush() in debug builds.
	bool validate(const World& w);

	// printf wrappers for quick use in a sandbox.
	void print(const World& w, Entity e);
	void printSummary(const World& w);

}