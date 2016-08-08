#include "VertexColorModel.hpp"

void VertexColorModel::update(double delta) {}

void VertexColorModel::draw() {
	glBindVertexArray(vertex_array_ID);
	{
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0  );
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
		glVertexAttribPointer(1, color_channel, GL_FLOAT,  GL_FALSE, 0, (void*)0   );

		for (auto it = instances.begin(); it != instances.end(); ++it) {
			const Instance& ins = it->second;
			if (!ins.show_up)
				continue;

			ins.base_set_up_shader(*this, ins);

			glDrawArrays(mode, 0, vertex_data.size());
		}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	} glBindVertexArray(0);
}