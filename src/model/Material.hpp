#ifndef MATERIAL
#define MATERIAL

class Material {
public:
	virtual ~Material() {}
	virtual void setUniforms(const vector<GLuint>& ids) const = 0;
};

class PhoneMaterial: public Material {
public:
	PhoneMaterial(const vec3& p_ambient_color_ratio, const vec3& p_specular_color, float p_specular_ratio):
		ambient_color_ratio{p_ambient_color_ratio}, specular_color{p_specular_color}, specular_ratio{p_specular_ratio} {}

	void setUniforms(const vector<GLuint>& ids) const {
		glUniform3f(ids[0], ambient_color_ratio.x, ambient_color_ratio.y, ambient_color_ratio.z);
		glUniform3f(ids[1], specular_color.x, specular_color.y, specular_color.z);
		glUniform1f(ids[2], specular_ratio);
	}

private:
	vec3 ambient_color_ratio;
	vec3 specular_color;
	float specular_ratio;
};

#endif