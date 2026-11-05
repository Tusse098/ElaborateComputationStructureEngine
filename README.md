# ElaborateComputationStructureEngine (ECSEngine)

This is my own effort in creating an ECS game engine.

A game engine that utilizes ECS structure to create 2D games similar to Unity DOTS (Unity ECS), a relatively new way of building games in Unity in a data-oriented design. Compared to the traditional object-oriented Unity we all know and, sometimes, love, Unity DOTS guides the developer toward better memory management and fewer cache misses. By separating the methods and members of classes into components for the data, and systems for the methods/functions, we store the same type of data close together and query through a single type or types of data in bulk. By also using archetypes to group multiple components into a single combined archetype we can do it even more effectively. This comes at a cost however: whenever we give an entity a new component it changes its archetype and therefore makes a structural change in the location of the memory, which is expensive. This is all fine and good, however, Unity is a 3D-first environment, and Unity DOTS is even more so, as it isn't yet equipped with 2D rendering out of the box. So instead I am making my own 2D focused, and worse, ECS game engine.

This is a learning project. The design notes below are the point as much as the code is.

## Status

The core container works and is tested:

- Component storage as sparse sets, one pool per component type
- Multi-component queries (`each<A, B, ...>`) that automatically iterate the smallest pool
- Generational entity handles with slot reuse, so stale handles are detectable

There are no systems, no scheduler, no events and no rendering yet. See [Roadmap](#roadmap).

## Example

```cpp
#include <ecs/core/world.h>

struct Position { float x, y; };
struct Velocity { float x, y; };

ecs::World w;

ecs::Entity e = w.create();
w.add(e, Position{ 0, 0 });
w.add(e, Velocity{ 1, 0 });

// Visits every entity that has BOTH components, in either argument order.
w.each<Position, Velocity>([](ecs::Entity, Position& p, Velocity& v) {
    p.x += v.x;
    p.y += v.y;
});

w.remove<Velocity>(e);
w.destroy(e);
w.alive(e);   // false - and stays false even after the slot is reused
```

## Design

### Storage: sparse sets

Each component type `T` lives in its own `Pool<T>`, which is a sparse set:

- `dense`    - the component data, packed contiguously
- `entities` - the owning entity of each `dense` slot (parallel array)
- `sparse`   - entity slot -> position in `dense`, or `NONE`

Add, remove and lookup are O(1). Remove is swap-and-pop, so `dense` never has holes and iteration is a linear walk over contiguous memory. Pools derive from a type-erased `IPool` so the world can hold them in one container and destroy an entity across all of them without knowing their `T`.

Unlike archetype-based designs (Unity DOTS, flecs), adding or removing a component touches only that one pool. No data moves between tables.

### Queries: iterate the smallest pool

`each<A, B, C>` needs the entities that have all three. The algorithm:

1. Pick the **lead**: the pool with the smallest `size()`. This is decided at runtime, once per call.
2. Walk the lead's `entities` array.
3. For each entity, probe the other pools with `has()`. If all say yes, fetch each component with `get()` and call the callback.

Visited count equals the lead's size, so choosing the smallest lead is the only optimisation that matters. The benchmark that motivated this, before lead selection was automatic (10,000,000 entities with `Position`, 1,000 with both `Position` and `Velocity`):

| Approach                        | Iterate (ms) | ns / visited |
| ------------------------------- | -----------: | -----------: |
| ECS, big pool first             |      5318.08 |         5.32 |
| ECS, small pool first           |         1.03 |        10.34 |
| OOP, Unity-style pointer chase  |     12543.49 |        12.54 |

Per-visit cost is *higher* when leading with the small pool (every probe into the big pool is a cache miss), but it visits 10,000x fewer entities. With runtime lead selection both argument orders now take the fast path.

Pool pointers are looked up once per `each` call and held in a tuple; nothing in the hot loop goes through the world's type-to-pool map.

### Entities: generational handles

`Entity` is a `uint32_t` packed as `[ generation : 8 bits ][ index : 24 bits ]`.

- **index** - the slot. Pools index their `sparse` array by this.
- **generation** - how many times the slot has been reused.

The world keeps one generation per slot and a free list of destroyed slots. `create()` pops a free slot and stamps the handle with that slot's current generation; `destroy()` removes the entity from every pool, then bumps the slot's generation and pushes it to the free list. `alive(e)` is true when the handle's generation matches the slot's current generation.

Why: without reuse, the id space and every `sparse` array grow with every entity ever created. With naive reuse, a system holding a handle to a destroyed entity silently starts referring to whatever took its slot. Generations make that handle fail `alive()` instead. A slot can be reused 256 times before the generation wraps.

Pools store full handles in `entities` but index `sparse` by `index(e)` only. Everything `each` hands to a callback is therefore a valid, `alive()` handle.

### Known limitations

These are deliberate gaps, not bugs:

- **No structural changes during iteration.** Calling `add`, `remove` or `destroy` inside an `each` callback is unsupported; swap-and-pop would reorder the array being walked.
- **No null entity.** `Entity{}` is slot 0, generation 0 - a real entity. "No entity" is not yet spellable.
- **Duplicate `add` is a debug assert**, not a replace.
- Generation wraparound after 256 reuses of one slot is not guarded.

## Project layout

```
ElaborateComputationStructureEngine/   the engine (header-only, include/ecs/...)
Sandbox/                               scratch executable for trying things
Bench/                                 benchmarks
Tests/                                 Catch2 unit tests
```

## Building

CMake with the Visual Studio generator, C++20. Catch2 comes from vcpkg.

1. Install Catch2: `vcpkg install catch2:x64-windows`
2. Point CMake at the vcpkg toolchain file. In `CMakePresets.json`:
   ```json
   "cacheVariables": {
     "CMAKE_TOOLCHAIN_FILE": "C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
   }
   ```
3. Configure and build.

Note: `project()` must come before any `find_package` in the root `CMakeLists.txt`. The toolchain file is loaded by `project()`; a `find_package` above it runs without vcpkg and fails to find Catch2.

## Design Decisions

### Compiled, not header-only

The engine builds as a static library. Templates (`Pool<T>`, `each`, the
command buffer's `add<T>`/`remove<T>`) live in headers because the language
requires it; everything else goes in `.cpp` files.

Header-only is the right call for a drop-in library like EnTT, where
adoption by strangers is the goal. This is an engine with exactly three
consumers (Sandbox, Bench, Tests) in one repo, so the trade runs the other
way: local recompiles and clean circular dependencies matter more than
single-include convenience.

### EntityCommandBuffer

Structural changes (`add`, `remove`, `destroy`) are not allowed inside `each`: they reorder the arrays being iterated. Record them instead and apply after:

```cpp
w.each<Health>([&](ecs::Entity e, Health& h) {
    if (h.hp <= 0) w.defer().destroy(e);
});
w.flush();
```

`create()` stays immediate. It never touches a pool, so it's safe anywhere.

Commands are stored as callables and replayed in recorded order. The world owns one buffer; `flush()` is called by whoever drives the loop.
EnTT made the opposite choice. It has no buffer: views iterate backwards, so destroying the current entity is safe, but destroying any other entity is not. A nuanced rule the engine can't enforce.
I chose to embrace the buffer. One rule, one tool, nothing that silently corrupts. The cost is an allocation per command, which is cheap next to iteration. Reverse iteration can be added later if a profile asks for it. For now, simple wins.

## Tests

Using Catch2 v3. I recommend installing Catch2 via vcpkg to make it easy for yourself.

All tests build into one executable, `ecs_tests`. Run them with any of:

- Visual Studio Test Explorer (Test -> Test Explorer -> Run All)
- `ctest --output-on-failure` from the build directory
- `ecs_tests` directly; `ecs_tests "[world]"` runs one tag

Test names are prefixed by unit (`Entity:`, `Pool:`, `World:`) and tagged the same way. Test Explorer caches test names; after renaming a test, a Visual Studio restart may be needed for the new name to appear.

## Roadmap

1. **Command buffer** - record `add`/`remove`/`destroy` during `each`, apply after. Needed for any system that spawns or kills. [x]
2. **Null entity** - a reserved handle that can never be alive. [x]
3. **Exclusion and tags** - `each<A, B>` excluding `C`; zero-size tag components.
4. **Systems** - a function list the world runs each frame.

I am planning on updating this as I go. I am also writing some documentation inside Obsidian which I will set up to have inside another git repo later.