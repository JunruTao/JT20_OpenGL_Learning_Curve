#include <GL/glew.h>
#include "GL_Create_Geometries.h"
#include "ObjectsCore.h"

void CreateGLGeometry(ObjectGeo &geo, GLuint *VAO, GLuint *VBO)
{

	
	glGenVertexArrays(1, VAO);            //Create a VAO array
	glBindVertexArray(*VAO);                //Bind VAO 

	glGenBuffers(1, VBO);                 //Create VBO buffer space
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);    //Bind this buffer

	//Getting the data from the object.
	glBufferData(
		GL_ARRAY_BUFFER,
		geo.GetGlVertices().size() * sizeof(GLfloat),
		geo.GetGlVertices().data(), GL_STATIC_DRAW);

	//Set Vertex Attribute Pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //Q: the first index is the context index?
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER,0); //Release Buffer (VBO)
	glBindVertexArray(0);            //Release vertice array (VAO)
}