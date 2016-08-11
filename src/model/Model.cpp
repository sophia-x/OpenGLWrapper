#include <stdio.h>
#include <stdlib.h>

#include "Model.hpp"

GLuint Model::loadTexture(const string &texture_path) {
	static const unsigned int FOURCC_DXT1{0x31545844}; // Equivalent to "DXT1" in ASCII
	static const unsigned int FOURCC_DXT3{0x33545844}; // Equivalent to "DXT3" in ASCII
	static const unsigned int FOURCC_DXT5{0x35545844}; // Equivalent to "DXT5" in ASCII

	/* try to open the file */
	FILE *fp = fopen(texture_path.c_str(), "rb");
	if (fp == NULL) {
		fprintf(stderr, "%s could not be opened.\n", texture_path.c_str());
		exit(EXIT_FAILURE);
	}

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		fprintf(stderr, "Not a DDS file.\n");
		exit(EXIT_FAILURE);
	}

	/* get the surface desc */
	unsigned char header[124];
	fread(&header, 124, 1, fp);

	unsigned int height      	= *(unsigned int*) & (header[8 ]);
	unsigned int width	     	= *(unsigned int*) & (header[12]);
	unsigned int linear_size 	= *(unsigned int*) & (header[16]);
	unsigned int mip_map_count 	= *(unsigned int*) & (header[24]);
	unsigned int four_cc      	= *(unsigned int*) & (header[80]);

	unsigned int bufsize = mip_map_count > 1 ? linear_size * 2 : linear_size;
	unsigned char *buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	fclose(fp);

	unsigned int components  = (four_cc == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (four_cc) {
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		fprintf(stderr, "Format not known\n");
		exit(EXIT_FAILURE);
	}

	// Create one OpenGL texture
	GLuint texture_ID;
	glGenTextures(1, &texture_ID);

	// "Bind" the texture
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int block_size = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mip_map_count && (width || height); ++level) {
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * block_size;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

		offset += size;
		width  /= 2;
		height /= 2;

		// Deal with Non-Power-Of-Two textures.
		if (width < 1) {
			width = 1;
		}
		if (height < 1) {
			height = 1;
		}
	}
	free(buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	return texture_ID;
}