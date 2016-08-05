#include <gui/WindowManager.hpp>

#include "SingleModel.hpp"

static const string STANDARD_SHADER_NAME  = "Standard_Shader";

void init_standard_shader(World *world) {
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
	world->addShader(STANDARD_SHADER_NAME, new Shader("shaders/StandardShading.vertexshader", "shaders/StandardShading.fragmentshader", names));
}

void standard_set_up_shader(const SingleModel &model, const Instance &ins) {
	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(STANDARD_SHADER_NAME);
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

	const vec3& position = ins.pos;
	const quat& origentation = ins.origentation;

	// Build the model matrix
	mat4 rotation_matrix = mat4_cast(origentation);
	mat4 translation_matrix = translate(mat4(), position);
	mat4 scaling_matrix = scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
	mat4 model_matrix = translation_matrix * rotation_matrix * scaling_matrix;

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