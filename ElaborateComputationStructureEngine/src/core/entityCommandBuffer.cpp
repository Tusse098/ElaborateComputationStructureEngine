// entityCommandBuffer.cpp
#include <ecs/core/entityCommandBuffer.h>
#include <ecs/core/world.h>

namespace ecs {

    void EntityCommandBuffer::destroy(Entity e) {
        cmds.push_back([e](World& w) { w.destroy(e); });
    }

    void EntityCommandBuffer::flush(World& w) {
        for (auto& c : cmds) c(w);
        cmds.clear();
    }
}