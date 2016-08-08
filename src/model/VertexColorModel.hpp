#ifndef __VERTEX_COLOR_MODEL_H
#define __VERTEX_COLOR_MODEL_H

#include <vector>
#include <string>
#include <map>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

#include "Model.hpp"
#include "World.hpp"

class VertexColorModel: public Model {
public:
	VertexColorModel(const vector<vec3> &p_vertex_data, const vector<GLfloat> &p_color_data, unsigned char p_color_channel, GLenum p_mode):
		vertex_data{p_vertex_data}, color_data{p_color_data}, color_channel{p_color_channel}, mode{p_mode}
	{
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size()*sizeof(vec3), &vertex_data[0], GL_DYNAMIC_DRAW);

		glGenBuffers(1, &color_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
		glBufferData(GL_ARRAY_BUFFER, color_data.size()*sizeof(GLfloat), &color_data[0], GL_DYNAMIC_DRAW);
	}

	~VertexColorModel() {
		glDeleteVertexArrays(1, &vertex_buffer);
		glDeleteVertexArrays(1, &color_buffer);
	}

	inline void addInstance(const string &name, const Instance &ins) {
		instances[name] = ins;
	}

	void update(double delta);
	void draw();

private:
	vector<vec3> vertex_data;
	vector<GLfloat> color_data;
	unsigned char color_channel;
	GLenum mode;

	GLuint vertex_buffer, color_buffer;

	map<string, Instance> instances;
};

void init_vertex_color_shader(World *world, const string& vertex_path, const string& freg_path, const string &name);
void vertex_color_set_up_shader(const Model &model, const Instance &ins);

#endif