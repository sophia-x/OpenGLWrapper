#ifndef PARTICLE_MODEL_H
#define PARTICLE_MODEL_H

#include <vector>
#include <algorithm>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

#include "Model.hpp"

struct Particle {
	Particle(): life{ -1}, camera_dist{ -1}, forever {false} {}

	inline void update(float delta, const vec3 &camera_pos) {
		life -= delta;
		if (alive()) {
			speed += acc * delta;
			pos += speed * delta;
			camera_dist = distance(pos, camera_pos);
		}
	}

	inline bool alive() const {
		return forever || life > 0;
	}

	vec3 pos, speed, acc;
	vec4 color;
	float size;
	float life;
	float camera_dist;
	bool forever;
};

bool life_compare_heap(const Particle &a, const Particle &b);
bool dist_compare(const Particle *a, const Particle *b);

class ParticleModel;
void addRandomParticles(size_t num, ParticleModel &model);
void init_particle_shader(World *world, const string &vertex_path, const string &frag_path, const string &name);
void particle_set_up_shader(const ParticleModel &model);

class ParticleModel: public Model {
public:
	ParticleModel(const vector<GLfloat> &p_vertex, const string& p_shader_name,
	              void(*p_set_up_shader_ptr)(const ParticleModel &) = particle_set_up_shader, size_t p_max_num = 100000);

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

	inline GLuint getTexture() const {
		return texture;
	}

	inline const string &getShaderName() const {
		return shader_name;
	}

	void update(double delta);
	void draw();

private:
	const size_t MAX_NUM;
	const size_t MAX_POS_SIZE;
	const size_t MAX_COLOR_SIZE;

	vector<GLfloat> g_vertex_buffer_data;

	vector<Particle *> particle_ptrs;
	vector<Particle> particles;

	vector<GLfloat> g_particule_position_size_data;
	vector<GLubyte> g_particule_color_data;

	GLuint billboard_vertex_buffer, particles_position_buffer, particles_color_buffer;
	GLuint texture;

	void(*set_up_shader_ptr)(const ParticleModel &);
	string shader_name;
};

#endif