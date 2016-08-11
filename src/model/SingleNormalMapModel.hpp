#ifndef SINGLE_NORMAL_MAP_MODEL_H
#define SINGLE_NORMAL_MAP_MODEL_H

#include "SingleModel.hpp"

class SingleNormalMapModel: public SingleModel {
public:
	SingleNormalMapModel(const string &obj_path): SingleModel{obj_path} {
		tangents = vector<vec3>(vertices.size());
		bitangents = vector<vec3>(vertices.size());
		computeTangentBasis();

		glBindVertexArray(vertex_array_ID);
		{
			glGenBuffers(1, &tangentbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
			glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(vec3), &tangents[0], GL_STATIC_DRAW);

			glGenBuffers(1, &bitangentbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
			glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(vec3), &bitangents[0], GL_STATIC_DRAW);
		}
		glBindVertexArray(0);
	}

	virtual ~SingleNormalMapModel() {
		glDeleteBuffers(1, &tangentbuffer);
		glDeleteBuffers(1, &bitangentbuffer);
	}

	inline const vector<vec3>& getTangents() const {
		return tangents;
	}

	inline const vector<vec3>& getBitangents() const {
		return bitangents;
	}

private:
	void computeTangentBasis();

private:
	vector<vec3> tangents;
	vector<vec3> bitangents;

	GLuint tangentbuffer, bitangentbuffer;
};

#endif