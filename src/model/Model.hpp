#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
using namespace glm;

#include <GL/glew.h>

class Model {
public:
	Model() {
		glGenVertexArrays(1, &vertex_array_ID);
	}

	virtual ~Model() {
		glDeleteVertexArrays(1, &vertex_array_ID);
	}

	virtual void update(double delta) = 0;
	virtual void draw() = 0;

	GLuint loadTexture(const string &texture_path) const;

protected:
	GLuint vertex_array_ID;
};

struct Instance {
	Instance(const string &p_shader_name = "", void(*p_base_set_up_shader)(const Model&, const Instance&) = 0, const vec3 &p_pos = vec3(), const quat &p_orig = quat(),
	         const vec3 &p_size = vec3(1.0f), bool p_show_up = true): shader_name{p_shader_name}, pos{p_pos}, origentation{p_orig}, size{p_size}, show_up{p_show_up},
		base_set_up_shader{p_base_set_up_shader} {}

	inline mat4 getModelMatrix() const {
		mat4 rotation_matrix = mat4_cast(origentation);
		mat4 translation_matrix = translate(mat4(), pos);
		mat4 scaling_matrix = scale(mat4(), size);
		mat4 model_matrix = translation_matrix * rotation_matrix * scaling_matrix;

		return model_matrix;
	}

	vec3 pos;
	quat origentation;
	vec3 size;

	bool show_up;

	string shader_name;

	void(*base_set_up_shader)(const Model&, const Instance&);

};

#endif