#ifndef VERTEX_TEXTURE_MODEL_H
#define VERTEX_TEXTURE_MODEL_H

#include <map>
using namespace std;

#include "World.hpp"

#include "Model.hpp"

class VertexTextureModel;

struct VertexTextureInstance: public Instance {
	VertexTextureInstance() {}
	VertexTextureInstance(const string &p_shader_name, const string &p_texture_name, void(*p_set_up_shader)(const VertexTextureModel &,
	                      const VertexTextureInstance &), const vec3 &p_pos = vec3(), const quat &p_orig = quat(),
	                      const vec3 &p_size = vec3(1.0f), bool p_show_up = true): Instance{p_shader_name, 0, p_pos, p_orig, p_size, p_show_up},
		texture_name{p_texture_name}, set_up_shader{p_set_up_shader} {}

	string texture_name;
	float life_level;
	void(*set_up_shader)(const VertexTextureModel &, const VertexTextureInstance &);
};

class VertexTextureModel: public Model {
public:
	VertexTextureModel(const vector<GLfloat> &p_vertex_data, const vector<GLfloat> &p_uvs, GLuint p_mode): vertex_data{p_vertex_data},
		uvs{p_uvs}, mode{p_mode}
	{
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size()*sizeof(GLfloat), &vertex_data[0], GL_DYNAMIC_DRAW);

		glGenBuffers(1, &uv_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
		glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(GLfloat), &uvs[0], GL_DYNAMIC_DRAW);
	}

	~VertexTextureModel() {
		glDeleteVertexArrays(1, &vertex_buffer);
		glDeleteVertexArrays(1, &uv_buffer);
		for (auto it = textures.begin(); it != textures.end(); ++it)
			glDeleteTextures(1, &it->second);
	}

	inline void addTexture(const string &name, const string &path) {
		textures[name] = loadTexture(path);
	}

	inline void addInstance(const string &name, const VertexTextureInstance &ins) {
		instances[name] = ins;
	}

	inline void setLifeLevel(const string& name, float p_life) {
		instances[name].life_level = p_life;
	}

	inline GLuint getTexture(const string &name) const {
		return textures.at(name);
	}

	void update(double delta);
	void draw();

private:
	vector<GLfloat> vertex_data, uvs;
	GLuint vertex_buffer, uv_buffer;
	map<string, VertexTextureInstance> instances;
	map<string, GLuint> textures;

	GLuint mode;
};

void init_billboard_less_shader(World *world, const string& vertex_path, const string& freg_path, const string& name);
void billboard_less_set_up_shader(const VertexTextureModel &model, const VertexTextureInstance &ins);

void init_billboard_more_shader(World *world, const string& vertex_path, const string& freg_path, const string& name);
void billboard_more_set_up_shader(const VertexTextureModel &model, const VertexTextureInstance &ins);

#endif