#ifndef MODEL
#define MODEL

#include <vector>
#include <string>
using namespace std;

#include <GL/glew.h>

class Model {
public:
	Model() {
		glGenVertexArrays(1, &vertex_array_ID);
	}

	virtual ~Model() {
		glDeleteVertexArrays(1, &vertex_array_ID);
	}

	virtual void update(double delta) = 0;
	virtual void draw() = 0;

	GLuint loadTexture(const string &texture_path) const;

protected:
	GLuint vertex_array_ID;
};

#endif