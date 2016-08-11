#include <gui/WindowManager.hpp>

#include "SingleModel.hpp"
#include "VertexColorModel.hpp"
#include "VertexTextureModel.hpp"
#include "SingleTextureModel.hpp"
#include "DebugModel.hpp"
#include "TextModel.hpp"

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

void standard_set_up_shader(const Model &r_model, const Instance &r_ins) {
	const SingleModel& model = (const SingleModel&)r_model;
	const SingleModelInstance& ins = (const SingleModelInstance&)r_ins;

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
	const mat4 &model_matrix = ins.base_ptr->getModelMatrix();

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
	mat4 MVP = projection_matrix * view_matrix * ins.base_ptr->getModelMatrix();

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
void billboard_less_set_up_shader(const Model &r_model, const Instance &r_ins) {
	const VertexTextureModel& model = (const VertexTextureModel&)r_model;
	const VertexTextureInstance& ins = (const VertexTextureInstance&)r_ins;

	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(ins.shader_name);
	glUseProgram(shader.getID());

	const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
	const mat4 &projection_matrix = controller.getProjectionMatrix();
	const mat4 &view_matrix = controller.getViewMatrix();
	mat4 VP = projection_matrix * view_matrix;

	glUniformMatrix4fv(shader.getUniform("VP"), 1, GL_FALSE, &VP[0][0]);
	glUniform3fv(shader.getUniform("billboard_pos"), 1, &ins.base_ptr->pos[0]);
	glUniform2f(shader.getUniform("billboard_size"), ins.base_ptr->size[0], ins.base_ptr->size[1]);
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
void billboard_more_set_up_shader(const Model &r_model, const Instance &r_ins) {
	const VertexTextureModel& model = (const VertexTextureModel&)r_model;
	const VertexTextureInstance& ins = (const VertexTextureInstance&)r_ins;

	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(ins.shader_name);
	glUseProgram(shader.getID());

	const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
	const mat4 &projection_matrix = controller.getProjectionMatrix();
	const mat4 &view_matrix = controller.getViewMatrix();
	mat4 VP = projection_matrix * view_matrix;

	glUniform3f(shader.getUniform("camera_right_worldspace"), view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
	glUniform3f(shader.getUniform("camera_up_worldspace"), view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);
	glUniformMatrix4fv(shader.getUniform("VP"), 1, GL_FALSE, &VP[0][0]);
	glUniform3fv(shader.getUniform("billboard_pos"), 1, &ins.base_ptr->pos[0]);
	glUniform2f(shader.getUniform("billboard_size"), ins.base_ptr->size[0], ins.base_ptr->size[1]);
	glUniform3fv(shader.getUniform("border"), 1, &BORDER[0]);
	glUniform4fv(shader.getUniform("life_color"), 1, &COLOR[0]);
	glUniform1f(shader.getUniform("life_level"), ins.life_level);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.getTexture(ins.texture_name));
	glUniform1i(shader.getUniform("texture_in"), 0);
}

void init_shadow_map_simple_shader(World *world, const string& vertex_path, const string& freg_path, const string& name) {
	vector<string> names {
		"MVP",
		"DepthBiasMVP",
		"texture_in",
		"shadowMap",
		"light_color",
		"bias"
	};
	world->addShader(name, new Shader(vertex_path, freg_path, names));
}
void shadow_map_simple_set_up_shader(const Model &r_model, const Instance &r_ins) {
	const SingleTextureModel& model = (const SingleTextureModel&)r_model;
	const SingleModelInstance& ins = (const SingleModelInstance&)r_ins;

	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(ins.shader_name);
	glUseProgram(shader.getID());

	const mat4 &model_matrix = ins.base_ptr->getModelMatrix();
	const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
	const mat4 &projection_matrix = controller.getProjectionMatrix();
	const mat4 &view_matrix = controller.getViewMatrix();
	mat4 MVP = projection_matrix * view_matrix * model_matrix;
	glUniformMatrix4fv(shader.getUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

	mat4 bias_matrix(
	    0.5, 0.0, 0.0, 0.0,
	    0.0, 0.5, 0.0, 0.0,
	    0.0, 0.0, 0.5, 0.0,
	    0.5, 0.5, 0.5, 1.0
	);
	mat4 depth_bias_MVP = bias_matrix * model.getMVP();
	glUniformMatrix4fv(shader.getUniform("DepthBiasMVP"), 1, GL_FALSE, &depth_bias_MVP[0][0]);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.getTexture(ins.texture_name));
	glUniform1i(shader.getUniform("texture_in"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, model.getBufferTexture());
	glUniform1i(shader.getUniform("shadowMap"), 1);

	glUniform3fv(shader.getUniform("light_color"), 1,
	             &WindowManager::getWindowManager().getCurrentWorld().getLight(model.getLightName()).getLightColor()[0]);

	glUniform1f(shader.getUniform("bias"), 0.005);
}

void init_depth_shader(World *world, const string& vertex_path, const string& freg_path, const string& name) {
	vector<string> names {
		"depthMVP"
	};
	world->addShader(name, new Shader(vertex_path, freg_path, names));
}

void depth_shader_set_up(SingleTextureModel &model, const SingleModelInstance &ins) {
	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(model.getTextureShaderName());
	glUseProgram(shader.getID());

	const vec3& light_pos = WindowManager::getWindowManager().getCurrentWorld().getLight(model.getLightName()).getLightPos();

	// Compute the MVP matrix from the light's point of view
	mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
	mat4 depthViewMatrix = glm::lookAt(light_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * ins.base_ptr->getModelMatrix();

	glUniformMatrix4fv(shader.getUniform("depthMVP"), 1, GL_FALSE, &depthMVP[0][0]);

	model.setUpMVP(depthMVP);
}

void init_passthrough_shader(World *world, const string& vertex_path, const string& freg_path, const string& name) {
	vector<string> names {
		"rendered_texture"
	};
	world->addShader(name, new Shader(vertex_path, freg_path, names));
}

void passthrough_shader_set_up(const Model &r_model, const Instance &r_ins) {
	const VertexTextureModel& model = (const VertexTextureModel&)r_model;
	const VertexTextureInstance& ins = (const VertexTextureInstance&)r_ins;

	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(ins.shader_name);
	glUseProgram(shader.getID());

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.getTexture(ins.texture_name));
	glUniform1i(shader.getUniform("rendered_texture"), 0);
}

void init_shadow_map_standard_shader(World *world, const string& vertex_path, const string& freg_path, const string& name) {
	vector<string> names {
		"MVP",
		"V",
		"M",
		"depth_bias_MVP",
		"texture_in",
		"light_position_worldspace",
		"light_color",
		"light_power",
		"ambient_ratio",
		"material_specular_color",
		"pow_index",
		"bias_ratio",
		"shadowMap"
	};
	world->addShader(name, new Shader(vertex_path, freg_path, names));
}
void shadow_map_standard_set_up_shader(const Model &r_model, const Instance &r_ins) {
	const SingleTextureModel& model = (const SingleTextureModel&)r_model;
	const SingleModelInstance& ins = (const SingleModelInstance&)r_ins;

	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(ins.shader_name);
	glUseProgram(shader.getID());

	const mat4 &model_matrix = ins.base_ptr->getModelMatrix();
	const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
	const mat4 &projection_matrix = controller.getProjectionMatrix();
	const mat4 &view_matrix = controller.getViewMatrix();
	mat4 MVP = projection_matrix * view_matrix * model_matrix;
	glUniformMatrix4fv(shader.getUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(shader.getUniform("V"), 1, GL_FALSE, &view_matrix[0][0]);
	glUniformMatrix4fv(shader.getUniform("M"), 1, GL_FALSE, &model_matrix[0][0]);

	mat4 bias_matrix(
	    0.5, 0.0, 0.0, 0.0,
	    0.0, 0.5, 0.0, 0.0,
	    0.0, 0.0, 0.5, 0.0,
	    0.5, 0.5, 0.5, 1.0
	);
	mat4 depth_bias_MVP = bias_matrix * model.getMVP();
	glUniformMatrix4fv(shader.getUniform("depth_bias_MVP"), 1, GL_FALSE, &depth_bias_MVP[0][0]);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.getTexture(ins.texture_name));
	glUniform1i(shader.getUniform("texture_in"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, model.getBufferTexture());
	glUniform1i(shader.getUniform("shadowMap"), 1);

	glUniform3fv(shader.getUniform("light_color"), 1,
	             &WindowManager::getWindowManager().getCurrentWorld().getLight(model.getLightName()).getLightColor()[0]);

	glUniform1f(shader.getUniform("bias_ratio"), 0.005);

	WindowManager::getWindowManager().getCurrentWorld().getLight(model.getLightName()).setUniforms(vector<GLuint>
	{
		shader.getUniform("light_position_worldspace"),
		shader.getUniform("light_color"),
		shader.getUniform("light_power")
	});

	WindowManager::getWindowManager().getCurrentWorld().getMaterial(ins.material_name).setUniforms(vector<GLuint>
	{
		shader.getUniform("ambient_ratio"),
		shader.getUniform("material_specular_color"),
		shader.getUniform("pow_index")
	});
}

void init_debug_shader(World *world, const string& vertex_path, const string& freg_path, const string &name) {
	vector<string> names {
		"MVP",
		"COLOR"
	};
	world->addShader(name, new Shader(vertex_path, freg_path, names));
}
void debug_set_up_shader(const Model &r_model, const Instance &ins) {
	const DebugModel& model = (const DebugModel&)r_model;

	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(ins.shader_name);
	glUseProgram(shader.getID());

	const mat4 &model_matrix = ins.base_ptr->getModelMatrix();
	const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
	const mat4 &projection_matrix = controller.getProjectionMatrix();
	const mat4 &view_matrix = controller.getViewMatrix();
	mat4 MVP = projection_matrix * view_matrix * model_matrix;
	glUniformMatrix4fv(shader.getUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

	glUniform3fv(shader.getUniform("COLOR"), 1, &model.getColor()[0]);
}

void init_text_shader(World *world, const string& vertex_path, const string& freg_path, const string &name) {
	vector<string> names {
		"half_width",
		"half_height",
		"texture_in"
	};
	world->addShader(name, new Shader(vertex_path, freg_path, names));
}
void text_set_up_shader(const TextModel &model) {
	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(model.getShaderName());
	glUseProgram(shader.getID());

	glUniform1i(shader.getUniform("half_width"), model.getWidth() / 2);
	glUniform1i(shader.getUniform("half_height"), model.getHeight() / 2);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.getTextureID());
	glUniform1i(shader.getUniform("texture_in"), 0);
}

void init_normal_shader(World *world, const string &vertex_path, const string &freg_path, const string &name) {
	vector<string> names {
		"MVP",
		"V",
		"M",
		"MV3x3",
		"DiffuseTextureSampler",
		"NormalTextureSampler",
		"SpecularTextureSampler",
		"LightPosition_worldspace",
		"LightColor",
		"LightPower",
		"MaterialAmbientRatio",
		"SpecularRatio",
		"power_index"
	};
	world->addShader(name, new Shader(vertex_path, freg_path, names));
}

void normal_set_up_shader(const Model &r_model, const Instance &r_ins) {
	const SingleModel& model = (const SingleModel&)r_model;
	const SingleModelInstance& ins = (const SingleModelInstance&)r_ins;

	const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(ins.shader_name);
	glUseProgram(shader.getID());

	WindowManager::getWindowManager().getCurrentWorld().getLight(model.getLightName()).setUniforms(vector<GLuint>
	{
		shader.getUniform("LightPosition_worldspace"),
		shader.getUniform("LightColor"),
		shader.getUniform("LightPower")
	});

	WindowManager::getWindowManager().getCurrentWorld().getMaterial(ins.material_name).setUniforms(vector<GLuint>
	{
		shader.getUniform("MaterialAmbientRatio"),
		shader.getUniform("SpecularRatio"),
		shader.getUniform("power_index")
	});

	// Build the model matrix
	const mat4 &model_matrix = ins.base_ptr->getModelMatrix();
	const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
	const mat4 &projection_matrix = controller.getProjectionMatrix();
	const mat4 &view_matrix = controller.getViewMatrix();
	mat4 MVP = projection_matrix * view_matrix * model_matrix;

	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(shader.getUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(shader.getUniform("M"), 1, GL_FALSE, &model_matrix[0][0]);
	glUniformMatrix4fv(shader.getUniform("V"), 1, GL_FALSE, &view_matrix[0][0]);

	mat3 ModelView3x3Matrix = mat3(view_matrix * model_matrix);
	glUniformMatrix3fv(shader.getUniform("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.getTexture(ins.texture_name));
	glUniform1i(shader.getUniform("DiffuseTextureSampler"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, model.getTexture(ins.normal_texture));
	glUniform1i(shader.getUniform("NormalTextureSampler"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, model.getTexture(ins.specular_texture));
	glUniform1i(shader.getUniform("SpecularTextureSampler"), 2);
}