#ifndef SINGLE_TEXTURE_MODEL_H
#define SINGLE_TEXTURE_MODEL_H

#include "SingleModel.hpp"

class SingleTextureModel: public SingleModel {
public:
	SingleTextureModel(const string &obj_path, void(*texture_setter)(size_t), size_t p_size, void(*p_texture_shader_set_up)
	                   (SingleTextureModel&, const SingleModelInstance&), const string& p_texture_shader_name);

	virtual ~SingleTextureModel() {
		glDeleteFramebuffers(1, &frame_buffer);
		glDeleteTextures(1, &buffer_texture);
	}

	void render_texture();

	void draw();

	void setUpMVP(const mat4& mvp) {
		MVP = mvp;
	}

	GLuint getBufferTexture() const {
		return buffer_texture;
	}

	inline const string& getTextureShaderName() const {
		return texture_shader_name;
	}

	inline const mat4 getMVP() const {
		return MVP;
	}

private:
	void(*texture_shader_setter)(SingleTextureModel&, const SingleModelInstance&);

private:
	GLuint frame_buffer, buffer_texture;

	size_t size;

	string texture_shader_name;

	mat4 MVP;
};

void depth_map_set_up(size_t size);

void init_shadow_map_simple_shader(World *world, const string& vertex_path, const string& freg_path, const string& name);
void shadow_map_simple_set_up_shader(const SingleModel &model, const SingleModelInstance &ins);

void init_depth_shader(World *world, const string& vertex_path, const string& freg_path, const string& name);
void depth_shader_set_up(SingleTextureModel& model, const SingleModelInstance& ins);

void init_shadow_map_standard_shader(World *world, const string& vertex_path, const string& freg_path, const string& name);
void shadow_map_standard_set_up_shader(const SingleModel &model, const SingleModelInstance &ins);

#endif