#include <ecs/core/world.h>
#include <ecs/fwd.h>
#include "windowPlayground.h"
#include <ecs/platform/input.h>
#include <ecs/platform/window.h>
#include <string>
#include <random>

int randomGenerator(int, int);

void game::systems::windowPlayground(ecs::World& w, float dt) {
	ecs::platform::Input& input = w.resource<ecs::platform::Input>();
	if (input.mousePressed[ecs::platform::Left]) {
		ecs::Entity e = w.create();
		w.add<ecs::platform::WindowDesc>(e, ecs::platform::WindowDesc{
			.title = "Testers",
			.x = randomGenerator(1000, 200),
			.y = randomGenerator(1000, 200),
			.w = randomGenerator(400, 50),
			.h = randomGenerator(400, 50),
			});
	}
	else if (input.mousePressed[ecs::platform::Right] && w.alive(input.mouseWindow)) {
		w.defer().destroy(input.mouseWindow);
	}
}

int randomGenerator(int min, int max) {
	return (rand() % (max - min + 1) + min);
}