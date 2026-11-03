// include/ecs/core/world.h
#pragma once

#include <algorithm>
#include <cassert>
#include <memory>
#include <tuple>
#include <vector>

#include <ecs/core/entity.h>
#include <ecs/core/pool.h>
#include <ecs/core/entityCommandBuffer.h>

namespace ecs {

    class World {
    public:
        // --- entity lifecycle (bodies in world.cpp) ---------------------------

        // Hands out a fresh handle. Reuses a freed slot if one exists, with that
        // slot's current generation; otherwise opens a new slot at generation 0.
        // Safe to call during each(): it never touches a pool.
        Entity create();

        // True when the handle's generation matches the slot's current one.
        bool alive(Entity e) const;

        // Removes e from every pool, then invalidates every handle to this life
        // by bumping the slot's generation, and frees the slot. No-op if dead.
        void destroy(Entity e);

        // --- components (templates: must stay in the header) -------------------

        template<typename T>
        T& add(Entity e, T v) {
            Pool<T>& p = pool<T>();
            p.add(e, v);
            return p.get(e);
        }

        template<typename T>
        T& get(Entity e) {
            return pool<T>().get(e);
        }

        template<typename T>
        bool has(Entity e) {
            return pool<T>().has(e);
        }

        template<typename T>
        void remove(Entity e) {
            pool<T>().remove(e);
        }

        // Calls fn(entity, components...) for every entity that has ALL of Pools.
        //   1. look each pool up once;
        //   2. pick the smallest as the lead;
        //   3. walk the lead's entities and probe the other pools for each one.
        // Argument order does not matter. Structural changes (add/remove/destroy)
        // inside fn are not allowed; record them with defer() and flush() after.
        template<typename... Pools, typename Fn>
        void each(Fn fn) {
            auto ps = std::make_tuple(&pool<Pools>()...);

            const IPool* lead = std::min(
                { static_cast<const IPool*>(std::get<Pool<Pools>*>(ps))... },
                [](const IPool* a, const IPool* b) { return a->size() < b->size(); });

            for (Entity e : lead->owners()) {
                if ((std::get<Pool<Pools>*>(ps)->has(e) && ...))
                    fn(e, std::get<Pool<Pools>*>(ps)->get(e)...);
            }
        }

        // --- deferred structural changes ---------------------------------------

        EntityCommandBuffer& defer();
        void flush();

    private:
        template<typename T>
        Pool<T>& pool() {
            uint32_t id = typeId<T>();
            if (id >= pools.size()) pools.resize(id + 1);
            if (!pools[id]) pools[id] = std::make_unique<Pool<T>>();
            return static_cast<Pool<T>&>(*pools[id]);
        }

        std::vector<uint8_t>                 generations;  // current generation per slot
        std::vector<uint32_t>                freelist;     // slots available for reuse
        std::vector<std::unique_ptr<IPool>>  pools;        // indexed by typeId<T>()
        EntityCommandBuffer                  ECB;
    };

}