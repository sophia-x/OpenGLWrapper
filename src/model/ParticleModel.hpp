#ifndef PARTICLE_MODEL
#define PARTICLE_MODEL

#include <vector>
#include <algorithm>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

#include "Model.hpp"

struct Particle {
	Particle(): life{ -1}, camera_dist{ -1} {}

	inline void update(float delta, const vec3 &camera_pos) {
		life -= delta;
		if (alive()) {
			speed += acc * delta;
			pos += speed * delta;
			camera_dist = distance(pos, camera_pos);
		}
	}

	inline bool alive() const {
		return life > 0;
	}

	vec3 pos, speed, acc;
	vec4 color;
	float size;
	float life;
	float camera_dist;
};

bool life_compare_heap(const Particle &a, const Particle &b);
bool dist_compare(const Particle *a, const Particle *b);

class ParticleModel: public Model {
public:
	ParticleModel(size_t p_max_num = 100000);

	~ParticleModel() {
		glDeleteBuffers(1, &particles_color_buffer);
		glDeleteBuffers(1, &particles_position_buffer);
		glDeleteBuffers(1, &billboard_vertex_buffer);
		glDeleteTextures(1, &texture);
	}

	inline void addParticle(const Particle &particle) {
		particles.push_back(particle);
		push_heap(particles.begin(), particles.end(), life_compare_heap);
	}

	inline void setTexture(const string &name) {
		texture = loadTexture(name);
	}

	void update(double delta);
	void draw();

private:
	const size_t MAX_NUM;
	const size_t MAX_POS_SIZE;
	const size_t MAX_COLOR_SIZE;

	vector<Particle *> particle_ptrs;
	vector<Particle> particles;

	vector<GLfloat> g_particule_position_size_data;
	vector<GLubyte> g_particule_color_data;

	GLuint billboard_vertex_buffer, particles_position_buffer, particles_color_buffer;
	GLuint texture;
};

void addRandomParticles(size_t num, ParticleModel &model);

#endif