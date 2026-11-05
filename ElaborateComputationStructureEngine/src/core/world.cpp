// src/ecs/core/world.cpp
#include <ecs/core/world.h>

namespace ecs {

    Entity World::create() {
        if (!freelist.empty()) {
            uint32_t idx = freelist.back();
            freelist.pop_back();
            return make(idx, generations[idx]);
        }
        assert(generations.size() < INDEX_MASK && "entity limit reached");
        generations.push_back(0);
        return make(static_cast<uint32_t>(generations.size() - 1), 0);
    }

    bool World::alive(Entity e) const {
        uint32_t idx = index(e);
        if (idx >= generations.size()) return false;

        return generations[idx] == generation(e);
    }

    void World::destroy(Entity e) {
        if (!alive(e)) return;                 // already dead: don't double-free the slot

        for (auto& p : pools) if (p) p->remove(e);   // 1. components go first

        uint32_t idx = index(e);
        generations[idx]++;                    // 2. every handle to this life is now stale
        freelist.push_back(idx);               // 3. slot is available again
    }

    EntityCommandBuffer& World::defer() { return ECB; }

    void World::flush() { ECB.flush(*this); }

}