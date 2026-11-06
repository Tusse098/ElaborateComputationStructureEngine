#include <ecs/core/scheduler.h>
#include <ecs/core/world.h>

namespace ecs {

	void Scheduler::add(ecs::Scheduler::System s) {
		systems.push_back(std::move(s));
	}
		
	void Scheduler::tick(World& w, float dt) {
		for (auto& s : systems) s(w, dt);
		w.flush();
	}
	
}