// entityCommandBuffer.h

#pragma once
#include <functional>
#include <vector>
#include <ecs/core/entity.h>

namespace ecs {
    class World;
    class EntityCommandBuffer {
    public:

        template<typename T>
        void add(Entity e, T v) { cmds.push_back([e, v](auto& w) { w.add(e, v); }); }

        template<typename T>
        void remove(Entity e) { cmds.push_back([e](auto& w) { w.remove<T>(e); }); }

        void destroy(Entity e);

        void flush(World& w);

    private:
        std::vector<std::function<void(World&)>> cmds;
    };
}