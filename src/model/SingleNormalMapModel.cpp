#include "SingleNormalMapModel.hpp"

void SingleNormalMapModel::computeTangentBasis() {
	for (int i = 0; i < indices.size(); i += 3) {
		// Shortcuts for vertices
		const vec3 & v0 = vertices[indices[i]];
		const vec3 & v1 = vertices[indices[i + 1]];
		const vec3 & v2 = vertices[indices[i + 2]];

		// Shortcuts for UVs
		const vec2 & uv0 = uvs[indices[i]];
		const vec2 & uv1 = uvs[indices[i + 1]];
		const vec2 & uv2 = uvs[indices[i + 2]];

		// Edges of the triangle : postion delta
		vec3 deltaPos1 = v1 - v0;
		vec3 deltaPos2 = v2 - v0;

		// UV delta
		vec2 deltaUV1 = uv1 - uv0;
		vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		vec3 tangent = normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r);
		vec3 bitangent = normalize((deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r);
		// vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		// vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		tangents[indices[i]] += tangent;
		bitangents[indices[i]] += bitangent;
		tangents[indices[i + 1]] += tangent;
		bitangents[indices[i + 1]] += bitangent;
		tangents[indices[i + 2]] += tangent;
		bitangents[indices[i + 2]] += bitangent;
	}

	for (unsigned int i = 0; i < vertices.size(); i ++) {
		const vec3& n = normals[i];
		vec3& t = tangents[i];
		const vec3& b = bitangents[i];

		// Gram-Schmidt orthogonalize
		t = glm::normalize(t - n * glm::dot(n, t));

		// Calculate handedness
		if (glm::dot(glm::cross(n, t), b) < 0.0f) {
			t = t * -1.0f;
		}
	}
}

void SingleNormalMapModel::pre_draw() {
	SingleModel::pre_draw();

	glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void SingleNormalMapModel::post_draw(){
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}