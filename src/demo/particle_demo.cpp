#include <gui/WindowManager.hpp>

#include <controllers/MouseCameraController.hpp>

#include <model/World.hpp>
#include <model/ParticleModel.hpp>

#include <tools/Timer.hpp>

#include "Demo.hpp"

#define TITLE "Particle"

void particle_demo() {
	WindowManager &manager = WindowManager::getWindowManager();

	WindowManager::getWindowManager().addWindow(NAME, createWindow(WIDTH, HEIGHT, TITLE, NAME));
	manager.setCurrent(NAME);
	manager.turnGlewOn();
	manager.addCurrentController(new MouseCameraController(manager.getCurrentWindow(), vec3(0, 0, 5), 45.0f, 3.14f, 0.0f, 3.0f, 0.005f, 0.5f));
	opengl_init(BG_COLOR);

	World *world = new World();
	manager.addWorld(NAME, world);
	manager.setCurrentWorld(NAME);

	ParticleModel* particle_model_ptr = new ParticleModel();
	particle_model_ptr->setTexture("textures/particle.DDS");
	world->addModel("Particles", particle_model_ptr);

	init_particle_shader(world);

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		manager.getCurrentController().control(delta);

		// Generate 10 new particule each millisecond,
		// but limit this to 16 ms (60 fps)
		{
			static float NUM_PER_SECOND = 10000.0;
			int num = (int)clamp(delta * NUM_PER_SECOND, 0.0, 0.016 * NUM_PER_SECOND);
			addRandomParticles(num, *particle_model_ptr);
		}

		world->update_draw(delta);
	}
}