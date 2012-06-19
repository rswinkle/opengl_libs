/*
 *(c) Robert Winkler
 *created 6/19/12
 *BSD license (see LICENSE)
 */

#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>


#define ATTRIBUTE_VERTEX	0

class Mesh
{
public:
	std::vector<glm::vec3> verts;

	
	GLenum primitive;
	GLuint bufferobjects[10];
	GLuint vbo, normal_vbo;		//vertexArrayBufferObject
	
	Mesh()
	{
		primitive = GL_POINTS;
	}
	Mesh(GLenum p)
	{
		primitive = p;
	}

	~Mesh()
	{
		;
	}
	
	
	void add_vertex(glm::vec3 a) { verts.push_back(a); }
	
	void add_vertex(float x, float y, float z) { verts.push_back(glm::vec3(x, y, z)); }

	

	void end();
	void draw();

};




#endif

