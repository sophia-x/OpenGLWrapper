#include <gui/WindowManager.hpp>

#include "SingleTextureModel.hpp"

SingleTextureModel::SingleTextureModel(const string &obj_path, void(*texture_setter)(size_t), size_t p_size,
                                       void(*p_texture_shader_setter)(SingleTextureModel&, const SingleModelInstance&), const string& p_texture_shader_name):
	SingleModel{obj_path}, size{p_size}, texture_shader_setter{p_texture_shader_setter}, texture_shader_name{p_texture_shader_name}
{
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

	glGenTextures(1, &buffer_texture);
	glBindTexture(GL_TEXTURE_2D, buffer_texture);

	texture_setter(size);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, buffer_texture, 0);

	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Cann't set up depth map!\n");
		exit(EXIT_FAILURE);
	}
}

void SingleTextureModel::render_texture() {
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	vector<GLint> border(4);
	glGetIntegerv(GL_VIEWPORT, &border[0]);
	glViewport(0, 0, size, size);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
	    0,
	    3,                            // size
	    GL_FLOAT,                     // type
	    GL_FALSE,                     // normalized?
	    0,                            // stride
	    (void*)0                      // array buffer offset
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	for (auto it = instances.begin(); it != instances.end(); ++it) {
		const SingleModelInstance& ins = it->second;
		if (!ins.base_ptr->show_up)
			continue;

		texture_shader_setter(*this, ins);
		// Draw the triangles !
		glDrawElements(
		    GL_TRIANGLES,      // mode
		    indices.size(),    // count
		    GL_UNSIGNED_SHORT, // type
		    (void*)0           // element array buffer offset
		);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(border[0], border[1], border[2], border[3]);
}

void SingleTextureModel::pre_draw() {
	render_texture();
	SingleModel::pre_draw();
}

void depth_map_set_up(size_t size) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
}