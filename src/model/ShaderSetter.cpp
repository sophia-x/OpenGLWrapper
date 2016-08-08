#include <gui/WindowManager.hpp>

#include "SingleModel.hpp"
#include "VertexColorModel.hpp"
#include "VertexTextureModel.hpp"

void init_standard_shader(World *world, const string &vertex_path, const string &freg_path, const string &name) {
	vector<string> names {
		"MVP",
		"V",
		"M",
		"light_position_worldspace",
		"texture_in",
		"light_color",
		"light_power",
		"material_ambient_color_ratio",
		"material_specular_color",
		"material_specular_ratio"
	};
	world->addShader(name, new Shader(vertex_path, freg_path, names));
}

void standard_set_up_shader(const SingleModel &model, const SingleModelInstance &ins) {
	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(ins.shader_name);
	glUseProgram(shader.getID());

	WindowManager::getWindowManager().getCurrentWorld().getLight(model.getLightName()).setUniforms(vector<GLuint>
	{
		shader.getUniform("light_position_worldspace"),
		shader.getUniform("light_color"),
		shader.getUniform("light_power")
	});

	WindowManager::getWindowManager().getCurrentWorld().getMaterial(ins.material_name).setUniforms(vector<GLuint>
	{
		shader.getUniform("material_ambient_color_ratio"),
		shader.getUniform("material_specular_color"),
		shader.getUniform("material_specular_ratio")
	});

	// Bind texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.getTexture(ins.texture_name));
	glUniform1i(shader.getUniform("texture_in"), 0);

	// Build the model matrix
	const mat4 &model_matrix = ins.getModelMatrix();

	const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
	const mat4 &projection_matrix = controller.getProjectionMatrix();
	const mat4 &view_matrix = controller.getViewMatrix();
	mat4 MVP = projection_matrix * view_matrix * model_matrix;

	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(shader.getUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(shader.getUniform("M"), 1, GL_FALSE, &model_matrix[0][0]);
	glUniformMatrix4fv(shader.getUniform("V"), 1, GL_FALSE, &view_matrix[0][0]);
}

void init_vertex_color_shader(World *world, const string& vertex_path, const string& freg_path, const string &name) {
	vector<string> names {
		"MVP"
	};
	world->addShader(name, new Shader(vertex_path, freg_path, names));
}

void vertex_color_set_up_shader(const Model &model, const Instance &ins) {
	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(ins.shader_name);
	glUseProgram(shader.getID());

	const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
	const mat4 &projection_matrix = controller.getProjectionMatrix();
	const mat4 &view_matrix = controller.getViewMatrix();
	mat4 MVP = projection_matrix * view_matrix * ins.getModelMatrix();

	glUniformMatrix4fv(shader.getUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);
}

void init_billboard_less_shader(World *world, const string& vertex_path, const string& freg_path, const string& name) {
	vector<string> names {
		"VP",
		"billboard_pos",
		"billboard_size",
		"texture_in",
		"border",
		"life_color",
		"life_level"
	};
	world->addShader(name, new Shader(vertex_path, freg_path, names));
}
const static vec3 BORDER(0.04, 0.3, 0.7);
const static vec4 COLOR(0.2, 0.8, 0.2, 1.0);
void billboard_less_set_up_shader(const VertexTextureModel &model, const VertexTextureInstance &ins) {
	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(ins.shader_name);
	glUseProgram(shader.getID());

	const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
	const mat4 &projection_matrix = controller.getProjectionMatrix();
	const mat4 &view_matrix = controller.getViewMatrix();
	mat4 VP = projection_matrix * view_matrix;

	glUniformMatrix4fv(shader.getUniform("VP"), 1, GL_FALSE, &VP[0][0]);
	glUniform3fv(shader.getUniform("billboard_pos"), 1, &ins.pos[0]);
	glUniform2f(shader.getUniform("billboard_size"), ins.size[0], ins.size[1]);
	glUniform3fv(shader.getUniform("border"), 1, &BORDER[0]);
	glUniform4fv(shader.getUniform("life_color"), 1, &COLOR[0]);
	glUniform1f(shader.getUniform("life_level"), ins.life_level);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.getTexture(ins.texture_name));
	glUniform1i(shader.getUniform("texture_in"), 0);
}

void init_billboard_more_shader(World *world, const string& vertex_path, const string& freg_path, const string& name) {
	vector<string> names {
		"camera_right_worldspace",
		"camera_up_worldspace",
		"VP",
		"billboard_pos",
		"billboard_size",
		"texture_in",
		"border",
		"life_color",
		"life_level"
	};
	world->addShader(name, new Shader(vertex_path, freg_path, names));
}
void billboard_more_set_up_shader(const VertexTextureModel &model, const VertexTextureInstance &ins) {
	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(ins.shader_name);
	glUseProgram(shader.getID());

	const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
	const mat4 &projection_matrix = controller.getProjectionMatrix();
	const mat4 &view_matrix = controller.getViewMatrix();
	mat4 VP = projection_matrix * view_matrix;

	glUniform3f(shader.getUniform("camera_right_worldspace"), view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
	glUniform3f(shader.getUniform("camera_up_worldspace"), view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);
	glUniformMatrix4fv(shader.getUniform("VP"), 1, GL_FALSE, &VP[0][0]);
	glUniform3fv(shader.getUniform("billboard_pos"), 1, &ins.pos[0]);
	glUniform2f(shader.getUniform("billboard_size"), ins.size[0], ins.size[1]);
	glUniform3fv(shader.getUniform("border"), 1, &BORDER[0]);
	glUniform4fv(shader.getUniform("life_color"), 1, &COLOR[0]);
	glUniform1f(shader.getUniform("life_level"), ins.life_level);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.getTexture(ins.texture_name));
	glUniform1i(shader.getUniform("texture_in"), 0);
}