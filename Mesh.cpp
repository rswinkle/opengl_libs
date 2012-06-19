/*
 *(c) Robert Winkler
 *created 6/19/12
 *BSD license (see LICENSE)
 */


#include "Mesh.h"
#include <stdio.h>

void Mesh::end()
{
	
	
	// Create the master vertex array object
	glGenVertexArrays(1, &vbo);
	glBindVertexArray(vbo);
    
 
	glGenBuffers(1, bufferobjects);

	// Vertex data
    glBindBuffer(GL_ARRAY_BUFFER, bufferobjects[0]);
	glEnableVertexAttribArray(ATTRIBUTE_VERTEX);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*verts.size()*3, &verts[0], GL_STATIC_DRAW);
	
	glVertexAttribPointer(ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);


	//add more bindings here for textures, colors, normals etc.

	
	// Done
	glBindVertexArray(0);
}


void Mesh::draw()
{
	glBindVertexArray(vbo);
	glDrawArrays(primitive, 0, verts.size());
	glBindVertexArray(0);

}







