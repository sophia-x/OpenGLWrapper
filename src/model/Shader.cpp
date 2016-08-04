#include <stdio.h>
#include <stdlib.h>

#include <fstream>
using namespace std;

#include "Shader.hpp"

Shader::Shader(const string &vertex_path, const string &fragment_path, vector<string> names) {
	GLuint vertex_shader_id = loadShader(vertex_path, GL_VERTEX_SHADER);
	GLuint fragment_shader_id = loadShader(fragment_path, GL_FRAGMENT_SHADER);

	// Link the program
	printf("Linking program\n");
	program_ID = glCreateProgram();
	glAttachShader(program_ID, vertex_shader_id);
	glAttachShader(program_ID, fragment_shader_id);
	glLinkProgram(program_ID);

	GLint result = GL_FALSE;
	int info_len;
	// Check the program
	glGetProgramiv(program_ID, GL_LINK_STATUS, &result);
	glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &info_len);
	if (info_len > 0) {
		vector<char> program_error_msg(info_len + 1);
		glGetProgramInfoLog(program_ID, info_len, NULL, &program_error_msg[0]);
		printf("%s\n", &program_error_msg[0]);
	}

	glDetachShader(program_ID, vertex_shader_id);
	glDetachShader(program_ID, fragment_shader_id);

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	if (result == GL_FALSE) {
		exit (EXIT_FAILURE);
	}

	for (const string &name : names) {
		uniforms[name] = glGetUniformLocation(program_ID, name.c_str());
	}
}

GLuint loadShader(const string &file_path, GLenum shader_type) {
	// Create the shaders
	GLuint shader_ID = glCreateShader(shader_type);

	// Read shader code from the file
	string shader_code;
	ifstream shader_stream(file_path, ios::in);
	if (shader_stream.is_open()) {
		string line = "";
		while (getline(shader_stream, line))
			shader_code += line;
		shader_stream.close();
	} else {
		fprintf(stderr, "Impossible to open %s.\n", file_path.c_str());
		exit(EXIT_FAILURE);
	}

	// Compile Shader
	printf("Compiling shader : %s\n", file_path.c_str());
	char const * source_ptr = shader_code.c_str();
	glShaderSource(shader_ID, 1, &source_ptr , NULL);
	glCompileShader(shader_ID);

	GLint result = GL_FALSE;
	int info_len;

	// Check Shader
	glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader_ID, GL_INFO_LOG_LENGTH, &info_len);
	if ( info_len > 0 ) {
		vector<char> shader_erroe_msg(info_len + 1);
		glGetShaderInfoLog(shader_ID, info_len, NULL, &shader_erroe_msg[0]);
		printf("%s\n", &shader_erroe_msg[0]);
	}
	if (result == GL_FALSE) {
		exit(EXIT_FAILURE);
	}

	return shader_ID;
}