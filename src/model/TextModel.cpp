#include "TextModel.hpp"

void TextModel::pre_draw() {
	glBindBuffer(GL_ARRAY_BUFFER, vertex_ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uv_ID);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_ID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uv_ID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
}

void TextModel::in_draw() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	set_up_shader(*this);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

	glDisable(GL_BLEND);
}

void TextModel::post_draw(){
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void TextModel::print_text(const string& text, int x, int y, int size) {
	for (size_t i = 0 ; i < text.size() ; i++ ) {
		vec2 vertex_up_left    = vec2( x + i * size     , y + size );
		vec2 vertex_up_right   = vec2( x + i * size + size, y + size );
		vec2 vertex_down_right = vec2( x + i * size + size, y      );
		vec2 vertex_down_left  = vec2( x + i * size     , y      );

		vertices.push_back(vertex_up_left   );
		vertices.push_back(vertex_down_left );
		vertices.push_back(vertex_up_right  );

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character % 16) / 16.0f;
		float uv_y = (character / 16) / 16.0f;

		vec2 uv_up_left    = vec2( uv_x           , uv_y );
		vec2 uv_up_right   = vec2( uv_x + 1.0f / 16.0f, uv_y );
		vec2 uv_down_right = vec2( uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f) );
		vec2 uv_down_left  = vec2( uv_x           , (uv_y + 1.0f / 16.0f) );
		uvs.push_back(uv_up_left   );
		uvs.push_back(uv_down_left );
		uvs.push_back(uv_up_right  );

		uvs.push_back(uv_down_right);
		uvs.push_back(uv_up_right);
		uvs.push_back(uv_down_left);
	}
}