#include <gui/WindowManager.hpp>

#include "SingleTextureModel.hpp"

SingleTextureModel::SingleTextureModel(const string &obj_path, void(*texture_setter)(vec2, GLuint), vec2 p_size,
                                       void(*p_texture_shader_setter)(SingleTextureModel&, const SingleModelInstance&), const string& p_texture_shader_name):
	SingleModel{obj_path}, size{p_size}, texture_shader_setter{p_texture_shader_setter}, texture_shader_name{p_texture_shader_name}
{
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

	glGenTextures(1, &buffer_texture);
	glBindTexture(GL_TEXTURE_2D, buffer_texture);

	texture_setter(size, buffer_texture);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Cann't set up depth map!\n");
		exit(EXIT_FAILURE);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SingleTextureModel::render_texture() {
	glBindVertexArray(1);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	vector<GLint> border(4);
	glGetIntegerv(GL_VIEWPORT, &border[0]);
	glViewport(0, 0, size[0], size[1]);

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
	glBindVertexArray(0);
}

void wood_map_set_up(vec2 size, GLuint buffer_texture) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size[0], size[1], 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size[0], size[1]);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, buffer_texture, 0);
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers);
}

void depth_map_set_up(vec2 size, GLuint buffer_texture) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, size[0], size[1], 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, buffer_texture, 0);
	glDrawBuffer(GL_NONE);
}