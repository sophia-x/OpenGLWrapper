#ifndef DEBUG_MODEL_H
#define DEBUG_MODEL_H

#include <vector>
#include <string>
#include <map>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

#include "Model.hpp"
#include "World.hpp"

class DebugModel: public Model {
public:
	DebugModel(const vec4& p_color, GLenum p_mode):
		color{p_color}, mode{p_mode}
	{
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	}

	~DebugModel() {
		glDeleteVertexArrays(1, &vertex_buffer);
	}

	inline void addInstance(const string &name, const Instance &ins) {
		instances[name] = ins;
	}

	inline void addVertex(const vec3& vertex) {
		vertex_data.push_back(vertex);
	}

	inline void update(double delta) {}
	inline virtual void pre_draw() {
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(vec3), &vertex_data[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	inline virtual void in_draw() {
		for (auto it = instances.begin(); it != instances.end(); ++it) {
			const Instance& ins = it->second;
			if (!ins.base_ptr->show_up)
				continue;

			ins.base_set_up_shader(*this, ins);

			glDrawArrays(mode, 0, vertex_data.size());
		}
	}

	inline virtual void post_draw() {
		glDisableVertexAttribArray(0);
	}

	inline const vec4& getColor() const {
		return color;
	}

private:
	vector<vec3> vertex_data;
	vec4 color;
	GLenum mode;

	GLuint vertex_buffer;

	map<string, Instance> instances;
};

void init_debug_shader(World *world, const string& vertex_path, const string& freg_path, const string &name);
void debug_set_up_shader(const Model &model, const Instance &ins);

#endif