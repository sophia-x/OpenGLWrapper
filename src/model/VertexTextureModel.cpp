#include "VertexTextureModel.hpp"

void VertexTextureModel::update(double delta) {}

void VertexTextureModel::pre_draw() {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(
	    0,                  // attribute
	    3,                  // size
	    GL_FLOAT,           // type
	    GL_FALSE,           // normalized?
	    0,                  // stride
	    (void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
	glVertexAttribPointer(
	    1,                  // attribute
	    2,                  // size
	    GL_FLOAT,           // type
	    GL_FALSE,           // normalized?
	    0,                  // stride
	    (void*)0            // array buffer offset
	);
}

void VertexTextureModel::in_draw() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (auto it = instances.begin(); it != instances.end(); ++it) {
		const VertexTextureInstance& ins = it->second;
		if (!ins.base_ptr->show_up)
			continue;

		ins.base_set_up_shader(*this, ins);

		glDrawArrays(mode, 0, vertex_data.size());
	}

	glDisable(GL_BLEND);
}

void VertexTextureModel::post_draw() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}