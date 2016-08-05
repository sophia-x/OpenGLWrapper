#ifndef NORMAL_MODEL
#define NORMAL_MODEL

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
struct Instance;

struct Instance {
	Instance() {}
	Instance(const string &p_texture_name, const string &p_material_name, void(*p_set_up_shader)(const SingleModel &, const Instance &),
	         vec3 p_pos = vec3(), quat p_orig = quat(), bool p_show_up = true): texture_name{p_texture_name}, material_name{p_material_name},
		set_up_shader{p_set_up_shader}, pos{p_pos}, origentation{p_orig}, show_up{p_show_up} {}

	vec3 pos;
	quat origentation;

	string texture_name;
	string material_name;
	void(*set_up_shader)(const SingleModel &, const Instance &);

	bool show_up;
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

	void update(double delta);
	void draw();

	inline void addTexture(const string &name, const string &path) {
		textures[name] = loadTexture(path);
	}

	inline void addInstance(const string &name, const Instance &instance) {
		instances[name] = instance;
	}

	inline void setPos(const string &name, const vec3& pos) {
		instances[name].pos = pos;
	}

	inline void setOrientation(const string &name, const quat& orig) {
		instances[name].origentation = orig;
	}

	inline void setShowUp(const string &name, bool show) {
		instances[name].show_up = show;
	}

	inline void setLightName(const string& name) {
		light_name = name;
	}

	inline const string &getLightName() const {
		return light_name;
	}

	inline GLuint getTexture(const string &name) const {
		return textures.at(name);
	}

private:
	vector<unsigned short> indices;
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;

	map<string, GLuint> textures;

	GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer;

	string light_name;

	map<string, Instance> instances;
};

void init_standard_shader(World *world);
void standard_set_up_shader(const SingleModel &model, const Instance &ins);

#endif