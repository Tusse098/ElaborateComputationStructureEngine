#pragma once
#include <cstdint>

namespace ecs {
    using Entity = uint32_t;

    constexpr uint32_t INDEX_BITS = 24;
    constexpr uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1;


    constexpr uint32_t index(Entity e) { return e & INDEX_MASK;}
    constexpr uint32_t generation(Entity e) { return e >> INDEX_BITS; }
    constexpr Entity make(uint32_t idx, uint32_t gen) {
        return ((gen << INDEX_BITS) | idx);
    }
    inline uint32_t nextTypeId() {
        static uint32_t counter = 0;   // one shared counter for the whole program
        return counter++;              // hand out 0, then 1, then 2...
    }

    template<typename T>
    uint32_t typeId() {
        static const uint32_t id = nextTypeId();   // runs ONCE per T, first call only
        return id;
    }

}