#ifndef SINGLE_TEXTURE_MODEL_H
#define SINGLE_TEXTURE_MODEL_H

#include "SingleModel.hpp"

class SingleTextureModel: public SingleModel {
public:
	SingleTextureModel(const string &obj_path, void(*texture_setter)(vec2, GLuint), vec2 p_size, void(*p_texture_shader_set_up)
	                   (SingleTextureModel&, const SingleModelInstance&), const string& p_texture_shader_name);

	virtual ~SingleTextureModel() {
		glDeleteFramebuffers(1, &frame_buffer);
		glDeleteTextures(1, &buffer_texture);
	}

	void render_texture();

	inline virtual void update(double delta) {}

	void setUpMVP(const mat4& mvp) {
		MVP = mvp;
	}

	GLuint getBufferTexture() const {
		return buffer_texture;
	}

	GLuint getFrameBuffer() const {
		return frame_buffer;
	}

	inline const string& getTextureShaderName() const {
		return texture_shader_name;
	}

	inline const mat4 getMVP() const {
		return MVP;
	}

	inline const vec2& getSize() const {
		return size;
	}

private:
	void(*texture_shader_setter)(SingleTextureModel&, const SingleModelInstance&);

private:
	GLuint frame_buffer, buffer_texture;

	vec2 size;

	string texture_shader_name;

	mat4 MVP;
};

void depth_map_set_up(vec2 size, GLuint buffer_texture);

void wood_map_set_up(vec2 size, GLuint buffer_texture);

void init_shadow_map_simple_shader(World *world, const string& vertex_path, const string& freg_path, const string& name);
void shadow_map_simple_set_up_shader(const Model &model, const Instance& ins);

void init_depth_shader(World *world, const string& vertex_path, const string& freg_path, const string& name);
void depth_shader_set_up(SingleTextureModel& model, const SingleModelInstance& ins);

void init_shadow_map_standard_shader(World *world, const string& vertex_path, const string& freg_path, const string& name);
void shadow_map_standard_set_up_shader(const Model &model, const Instance &ins);

#endif