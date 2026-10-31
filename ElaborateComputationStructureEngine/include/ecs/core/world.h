#pragma once
#include <ecs/core/entity.h>
#include <vector>
#include <memory>
#include "pool.h"

namespace ecs {

class World {
public:
    Entity  create() {
        return nextEntity++;      // returns 0, then 1, then 2...
    }
    void    destroy(Entity e) {
        for (auto& p : pools)
        {
            if (p) p->remove(e);
        }
    }
    template<typename T> T& add(Entity e, T v) {
        Pool<T>& p = pool<T>();
        p.add(e, v);
        return p.get(e);
    }
    template<typename T> T& get(Entity e) {
        Pool<T>& p = pool<T>();
        return p.get(e);
    }
    template<typename T> bool has(Entity e) {
        Pool<T>& p = pool<T>();
        return p.has(e);
    }
    template<typename T> void remove(Entity e) {
        Pool<T>& p = pool<T>();
        p.remove(e);
    }

    template<typename... Pools, typename Fn> void each(Fn fn) {
        auto pools = std::make_tuple(&pool<Pools>()...);

        const IPool* lead = std::min(
            { static_cast<const IPool*>(std::get<Pool<Pools>*>(pools))... },
            [](const IPool* a, const IPool* b) { return a->size() < b->size(); });
        

        for (Entity e : lead->owners()) {
            if ((std::get<Pool<Pools>*>(pools)->has(e) && ...))
                fn(e, std::get<Pool<Pools>*>(pools)->get(e)...);
        }
    }


private:
    Entity nextEntity = 0;
    std::vector<std::unique_ptr<IPool>> pools;
    template<typename T> Pool<T>& pool() {
        uint32_t id = typeId<T>();
        if (id >= pools.size()) pools.resize(id + 1);
        if (!pools[id]) pools[id] = std::make_unique<Pool<T>>();
        return static_cast<Pool<T>&>(*pools[id]);
    }
};

}