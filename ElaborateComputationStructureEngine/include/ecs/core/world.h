#pragma once
#include <ecs/core/entity.h>

namespace ecs {

    class World {
    public:
        Entity create() {
            return nextEntity++;      // returns 0, then 1, then 2...
        }

    private:
        Entity nextEntity = 0;
    };

}