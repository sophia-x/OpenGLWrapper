#include <glm/gtc/random.hpp>

#include "gui/WindowManager.hpp"

#include "ParticleModel.hpp"

void init_particle_shader(World *world, const string &vertex_path, const string &frag_path, const string &name) {
	vector<string> names {
		"camera_right_worldspace",
		"camera_up_worldspace",
		"vp",
		"texture_in"
	};
	world->addShader(name, new Shader(vertex_path, frag_path, names));
}

void particle_set_up_shader(const ParticleModel &model) {
	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(model.getShaderName());
	glUseProgram(shader.getID());
	// Bind texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.getTexture());
	glUniform1i(shader.getUniform("texture_in"), 0);

	const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
	const mat4 &projection_matrix = controller.getProjectionMatrix();
	const mat4 &view_matrix = controller.getViewMatrix();
	mat4 view_projection_matrix = projection_matrix * view_matrix;

	glUniform3f(shader.getUniform("camera_right_worldspace"), view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
	glUniform3f(shader.getUniform("camera_up_worldspace"), view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);
	glUniformMatrix4fv(shader.getUniform("vp"), 1, GL_FALSE, &view_projection_matrix[0][0]);
}

ParticleModel::ParticleModel(const vector<GLfloat> &p_vertex, const string& p_shader_name,
                             void(*p_set_up_shader_ptr)(const ParticleModel &), size_t p_max_num):
	g_vertex_buffer_data{p_vertex}, shader_name{p_shader_name}, MAX_NUM{p_max_num}, set_up_shader_ptr{p_set_up_shader_ptr},
	MAX_POS_SIZE{p_max_num * 4 * sizeof(GLfloat)}, MAX_COLOR_SIZE(p_max_num * 4 * sizeof(GLubyte)), particles(0),
	g_particule_position_size_data(p_max_num * 4), particle_ptrs(p_max_num) , g_particule_color_data(p_max_num * 4) {

	particles.reserve(MAX_NUM);

	glBindVertexArray(vertex_array_ID);
	{
		glGenBuffers(1, &billboard_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size()*sizeof(GLfloat), &g_vertex_buffer_data[0], GL_STATIC_DRAW);

		// The VBO containing the positions and sizes of the particles
		glGenBuffers(1, &particles_position_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, MAX_POS_SIZE, NULL, GL_STREAM_DRAW);

		// The VBO containing the colors of the particles
		glGenBuffers(1, &particles_color_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, MAX_COLOR_SIZE, NULL, GL_STREAM_DRAW);
	}
	glBindVertexArray(0);
}

void ParticleModel::update(double delta) {
	const vec3& camera_pos = WindowManager::getWindowManager().getCurrentController().getCameraPosition();

	for (size_t i = 0; i < particles.size(); i ++) {
		Particle &p = particles[i];
		p.update(delta, camera_pos);
	}

	while (!particles.empty() && !particles[0].alive()) {
		pop_heap(particles.begin(), particles.end(), life_compare_heap);
		particles.pop_back();
	}

	for (size_t i = 0; i < particles.size(); i ++)	{
		particle_ptrs[i] = &particles[i];
	}

	sort(particle_ptrs.begin(), particle_ptrs.begin() + particles.size(), dist_compare);

	for (size_t i = 0; i < particles.size(); i ++) {
		const Particle &p = *particle_ptrs[i];
		if (!p.alive())
			continue;

		// Fill the GPU buffer
		g_particule_position_size_data[4 * i + 0] = p.pos.x;
		g_particule_position_size_data[4 * i + 1] = p.pos.y;
		g_particule_position_size_data[4 * i + 2] = p.pos.z;
		g_particule_position_size_data[4 * i + 3] = p.size;

		g_particule_color_data[4 * i + 0] = p.color.x;
		g_particule_color_data[4 * i + 1] = p.color.y;
		g_particule_color_data[4 * i + 2] = p.color.z;
		g_particule_color_data[4 * i + 3] = p.color.w;
	}
}

void ParticleModel::draw() {
	glBindVertexArray(vertex_array_ID);
	{
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, MAX_POS_SIZE, NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, g_particule_position_size_data.size() * sizeof(GLfloat), &g_particule_position_size_data[0]);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, MAX_COLOR_SIZE, NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, g_particule_color_data.size() * sizeof(GLubyte), &g_particule_color_data[0]);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		set_up_shader_ptr(*this);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(
		    0,                  // attribute
		    3,                  // size
		    GL_FLOAT,           // type
		    GL_FALSE,           // normalized?
		    0,                  // stride
		    (void*)0            // array buffer offset
		);

		// 2nd attribute buffer : positions of particles' centers
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glVertexAttribPointer(
		    1,                                // attribute.
		    4,                                // size : x + y + z + size => 4
		    GL_FLOAT,                         // type
		    GL_FALSE,                         // normalized?
		    0,                                // stride
		    (void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : particles' colors
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glVertexAttribPointer(
		    2,                                // attribute.
		    4,                                // size : r + g + b + a => 4
		    GL_UNSIGNED_BYTE,                 // type
		    GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
		    0,                                // stride
		    (void*)0                          // array buffer offset
		);

		glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisor(1, 1); // positions : one per quad                -> 1
		glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particles.size());


		glDisable(GL_BLEND);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		glUseProgram(0);
	}
	glBindVertexArray(0);
}

void addRandomParticles(size_t num, ParticleModel &model) {
	Particle p;

	for (size_t i = 0; i < num; i ++) {
		p.life = 5.0f;
		p.pos = vec3(0, 0, -20.0f);
		p.speed = vec3(0.0f, 10.0f, 0.0f) + ballRand(1.5f);
		p.acc = vec3(0.0f, -9.81f, 0.0f);

		p.color = linearRand(vec4(0), vec4(vec3(256), 256 / 3));
		p.size = linearRand(0.1, 0.6);

		model.addParticle(p);
	}
}

bool life_compare_heap(const Particle &a, const Particle &b) {
	return a.life > b.life;
}

bool dist_compare(const Particle *a, const Particle *b) {
	return a->camera_dist > b->camera_dist;
}