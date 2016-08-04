#ifndef SHADER
#define SHADER

#include <string>
#include <vector>
#include <map>
using namespace std;

#include <GL/glew.h>

class Shader {
public:
	Shader(const string &vertex_path, const string &fragment_path, vector<string> names);
	~Shader() {
		glDeleteProgram(program_ID);
	}

	inline GLuint getID() const {
		return program_ID;
	}

	inline GLuint getUniform(const string &name) const {
		return uniforms.at(name);
	}

private:
	GLuint program_ID;
	map<string, GLuint> uniforms;
};

GLuint loadShader(const string &file_path, GLenum shader_type);

#endif