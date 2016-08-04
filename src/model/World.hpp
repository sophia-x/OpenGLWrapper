#ifndef WORLD
#define WORLD

#include <map>
#include <string>
#include <memory>
using namespace std;

#include "Model.hpp"
#include "Shader.hpp"

class World {
public:
	inline void addModel(const string &name, Model *model) {
		models[name] = unique_ptr<Model>(model);
	}

	inline void addShader(const string &name, Shader *shader) {
		shaders[name] = unique_ptr<Shader>(shader);
	}

	inline const Shader& getShader(const string& name) {
		return *shaders[name];
	}

private:
	map<string, unique_ptr<Model> > models;
	map<string, unique_ptr<Shader> > shaders;
};

#endif