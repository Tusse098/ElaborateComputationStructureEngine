// include/ecs/fwd.h
#pragma once
#include <cstdint>

namespace ecs {
    using Entity = uint32_t;
    class World;
    class Scheduler;
    class EntityCommandBuffer;
    struct IPool;
    template<typename T> class Pool;
}