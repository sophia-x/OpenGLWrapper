#ifndef WORLD
#define WORLD

#include <map>
#include <string>
#include <memory>
using namespace std;

#include "Model.hpp"
#include "Shader.hpp"
#include "Light.hpp"
#include "Material.hpp"

class World {
public:
	inline void addModel(const string &name, Model *model) {
		models[name] = unique_ptr<Model>(model);
	}

	inline void addShader(const string &name, Shader *shader) {
		shaders[name] = unique_ptr<Shader>(shader);
	}

	inline void addLight(const string &name, Light *light) {
		lights[name] = unique_ptr<Light>(light);
	}

	inline void addMaterial(const string &name, Material *material) {
		materials[name] = unique_ptr<Material>(material);
	}

	inline const Shader& getShader(const string& name) {
		return *shaders[name];
	}

	inline const Light& getLight(const string& name) {
		return *lights[name];
	}

	inline const Material& getMaterial(const string& name) {
		return *materials[name];
	}

	inline void update_draw(double delta) {
		for (auto it = models.begin(); it != models.end(); ++it) {
			Model &model = *(it->second);
			model.update(delta);
			model.draw();
		}
	}

private:
	map<string, unique_ptr<Model> > models;
	map<string, unique_ptr<Shader> > shaders;
	map<string, unique_ptr<Light> > lights;
	map<string, unique_ptr<Material> > materials;
};

#endif