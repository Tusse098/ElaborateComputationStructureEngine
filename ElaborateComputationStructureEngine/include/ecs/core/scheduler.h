// include/ecs/core/scheduler.h
#pragma once

#include <vector>
#include <functional>
#include <ecs/fwd.h>


namespace ecs { 
    class Scheduler {
    public:
        using System = std::function<void(World&, float)>;
        void add(System s);                 // registration order = run order
        void tick(World& w, float dt);      // run all, then w.flush()
    private:
        std::vector<System> systems;
    };

}