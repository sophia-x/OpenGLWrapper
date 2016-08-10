#ifndef NORMAL_MODEL_H
#define NORMAL_MODEL_H

#include <vector>
#include <string>
#include <map>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
using namespace glm;

#include "World.hpp"
#include "Model.hpp"

class SingleModel;

struct SingleModelInstance: Instance {
	SingleModelInstance() {}
	SingleModelInstance(const string& p_shader_name, const string &p_texture_name, const string &p_material_name,
	                    void(*p_set_up_shader)(const SingleModel &, const SingleModelInstance &), const vec3 &p_pos = vec3(),
	                    const quat &p_orig = quat(), const vec3 &p_size = vec3(1.0f), bool p_show_up = true):
		Instance{p_shader_name, 0, p_pos, p_orig, p_size, p_show_up}, texture_name{p_texture_name}, material_name{p_material_name},
		set_up_shader{p_set_up_shader} {}

	string texture_name;
	string material_name;
	void(*set_up_shader)(const SingleModel &, const SingleModelInstance &);
};

class SingleModel: public Model {
public:
	SingleModel(const string &obj_path);

	virtual ~SingleModel() {
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteBuffers(1, &uvbuffer);
		glDeleteBuffers(1, &normalbuffer);
		glDeleteBuffers(1, &elementbuffer);

		for (auto it = textures.begin(); it != textures.end(); ++it)
			glDeleteTextures(1, &it->second);
	}

	virtual void update(double delta);
	virtual void draw();

	inline void useVertexBuffer() {
		glBindVertexArray(vertex_array_ID);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
		    0,
		    3,                            // size
		    GL_FLOAT,                     // type
		    GL_FALSE,                     // normalized?
		    0,                            // stride
		    (void*)0                      // array buffer offset
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	}

	inline void addTexture(const string & name, const string & path) {
		textures[name] = loadTexture(path);
	}

	inline void addInstance(const string & name, const SingleModelInstance & instance) {
		instances[name] = instance;
	}

	inline void setPos(const string & name, const vec3 & pos) {
		instances[name].pos = pos;
	}

	inline void setOrientation(const string & name, const quat & orig) {
		instances[name].origentation = orig;
	}

	inline void setShowUp(const string & name, bool show) {
		instances[name].show_up = show;
	}

	inline void setLightName(const string & name) {
		light_name = name;
	}

	inline const string &getLightName() const {
		return light_name;
	}

	inline GLuint getTexture(const string & name) const {
		return textures.at(name);
	}

	inline map<string, SingleModelInstance>& getInstances() {
		return instances;
	}

	inline void draw_mesh() {
		// Draw the triangles !
		glDrawElements(
		    GL_TRIANGLES,      // mode
		    indices.size(),    // count
		    GL_UNSIGNED_SHORT, // type
		    (void*)0           // element array buffer offset
		);
	}
protected:
	vector<unsigned short> indices;
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;

	map<string, GLuint> textures;

	GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer;

	string light_name;

	map<string, SingleModelInstance> instances;
};

void init_standard_shader(World *world, const string &vertex_path, const string &freg_path, const string &name);
void standard_set_up_shader(const SingleModel &model, const SingleModelInstance &ins);

#endif