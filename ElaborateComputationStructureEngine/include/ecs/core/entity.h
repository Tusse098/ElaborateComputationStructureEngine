#pragma once
#include <cstdint>

namespace ecs {
    using Entity = uint32_t;

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