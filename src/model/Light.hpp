#ifndef LIGHT_H
#define LIGHT_H

#include <GL/glew.h>

#include <glm/glm.hpp>
using namespace glm;

class Light {
public:
	virtual ~Light() {}

	virtual void setUniforms(const vector<GLuint>& ids) const = 0;
};

class PointLight: public Light {
public:
	PointLight(const vec3 &p_light_pos, const vec3 &p_light_color, float p_light_power):
		light_pos{p_light_pos}, light_color{p_light_color}, light_power{p_light_power} {}

	void setUniforms(const vector<GLuint>& ids) const {
		glUniform3f(ids[0], light_pos.x, light_pos.y, light_pos.z);
		glUniform3f(ids[1], light_color.x, light_color.y, light_color.z);
		glUniform1f(ids[2], light_power);
	}

private:
	vec3 light_pos;
	vec3 light_color;
	float light_power;
};

#endif