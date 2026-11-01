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
        /*
         So we make a pool with Pool as the "component".
         No first we make a tuple... what does that mean? But the tuple get an Entity as the value as we for some reason can use pools inside the get function?
         But we know pools is a tyuple which is pretty much a list.
         So this tuple pools is a kind of list with "&pool<Pools>()..." So this is the pool function, but what does it do here?
         My guess is that it takes all the pools from the pack, one at a time(?) or parallel(?), and it returns a list of the adresses to each pool?  return static_cast<Pool<T>&>(*pools[id]);
         Because we are casting from *pools[id] which should be a dereference of an element inside "std::vector<std::unique_ptr<IPool>>".
         Which means it is the value (Or is it the reference) of an IPool.
         So we are casting from this IPool to Pool<T>&>. Which is a reference of a Pool.
         So we are getting a tuple of all the pools where the Pools pack values are the type.
         That makes sense, that is literally what we want.
         Nope that is not it, it is giving us the Entities of each pool? Huh? Is it not? Because we need to send an Entity insnide the get function down below, but it doesn't seem to be an Entity...

         Now what the fuck is this?
         const IPool* lead = std::min(
            { static_cast<const IPool*>(std::get<Pool<Pools>*>(pools))... },
            [](const IPool* a, const IPool* b) { return a->size() < b->size(); });

         Yeah I have no idea.
         It seems like it is using a callback on the bottom.
         Ahh nevermind, we again are using the pack where it will take all the values from the pack in the get get<Pool<Pools>*>(pools). 

        */
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