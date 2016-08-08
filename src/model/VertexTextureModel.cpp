#include "VertexTextureModel.hpp"

void VertexTextureModel::update(double delta) {}

void VertexTextureModel::draw() {
	glBindVertexArray(vertex_array_ID);
	{
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

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (auto it = instances.begin(); it != instances.end(); ++it) {
			const VertexTextureInstance& ins = it->second;
			if (!ins.show_up)
				continue;

			ins.set_up_shader(*this, ins);

			glDrawArrays(mode, 0, vertex_data.size());
		}

		glDisable(GL_BLEND);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
	glBindVertexArray(0);
}