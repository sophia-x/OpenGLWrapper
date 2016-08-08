#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <gui/WindowManager.hpp>

#include <tools/quaternion.hpp>

#include "Shader.hpp"
#include "SingleModel.hpp"

SingleModel::SingleModel(const string &path) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_SortByPType);
	if (!scene || !scene->HasMeshes()) {
		fprintf(stderr, "%s", importer.GetErrorString());
		exit(EXIT_FAILURE);
	}

	const aiMesh* mesh = scene->mMeshes[0];

	// Fill vertices positions
	vertices.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D pos = mesh->mVertices[i];
		vertices.push_back(vec3(pos.x, pos.y, pos.z));
	}

	// Fill vertices texture coordinates
	if (mesh->HasTextureCoords(0)) {
		uvs.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D uvw = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords;
			uvs.push_back(vec2(uvw.x, uvw.y));
		}
	} else {
		uvs = vector<vec2>(mesh->mNumVertices, vec2(0));
	}

	// Fill vertices normals
	normals.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D n = mesh->mNormals[i];
		normals.push_back(vec3(n.x, n.y, n.z));
	}

	// Fill face indices
	indices.reserve(3 * mesh->mNumFaces);
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		if (mesh->mFaces[i].mNumIndices != 3)
			continue;

		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	glBindVertexArray(vertex_array_ID);
	{
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &uvbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);

		glGenBuffers(1, &normalbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

	}
	glBindVertexArray(0);
}

void SingleModel::update(double delta) {}

void SingleModel::draw() {
	glBindVertexArray(vertex_array_ID);
	{
		// 1rst attribute buffer : vertices
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

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
		    1,
		    2,                            // size : U+V => 2
		    GL_FLOAT,                     // type
		    GL_FALSE,                     // normalized?
		    0,                            // stride
		    (void*)0                      // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
		    2,
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
			if (!ins.show_up)
				continue;

			ins.set_up_shader(*this, ins);

			// Draw the triangles !
			glDrawElements(
			    GL_TRIANGLES,      // mode
			    indices.size(),    // count
			    GL_UNSIGNED_SHORT, // type
			    (void*)0           // element array buffer offset
			);
		}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		glUseProgram(0);
	}
	glBindVertexArray(0);
}