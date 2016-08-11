#ifndef TEXT_MODEL_H
#define TEXT_MODEL_H

#include "Model.hpp"
#include "World.hpp"

class TextModel: public Model {
public:
	TextModel(size_t p_width, size_t p_height, const string& texture_path, const string& p_shader_name,
	          void(*p_set_up_shader)(const TextModel&)): shader_name{p_shader_name},
		width{p_width}, height{p_height}, set_up_shader{p_set_up_shader}
	{
		texture_ID = loadTexture(texture_path);

		glGenBuffers(1, &vertex_ID);
		glGenBuffers(1, &uv_ID);
	}

	virtual ~TextModel() {
		glDeleteTextures(1, &texture_ID);
		glDeleteBuffers(1, &vertex_ID);
		glDeleteBuffers(1, &uv_ID);
	}

	virtual inline void update(double delta) {}

	virtual void pre_draw();
	virtual void in_draw();
	virtual void post_draw();

	void print_text(const string& text, int x, int y, int size);

	inline void clear() {
		vertices.clear();
		uvs.clear();
	}

	inline const string& getShaderName() const {
		return shader_name;
	}

	inline size_t getWidth() const {
		return width;
	}

	inline size_t getHeight() const {
		return height;
	}

	inline GLuint getTextureID() const {
		return texture_ID;
	}

private:
	GLuint texture_ID, vertex_ID, uv_ID;
	string shader_name;
	size_t width, height;
	void(*set_up_shader)(const TextModel&);

	vector<vec2> vertices, uvs;
};

void init_text_shader(World *world, const string& vertex_path, const string& freg_path, const string &name);
void text_set_up_shader(const TextModel &model);

#endif