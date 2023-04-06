#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>


#include "Buffer.h"


void VertexBuffer::CreateBuffer(GLsizeiptr bufferSize_B, const void* data)
{		

	// VAO probably with shaders / or in fn thT CALLS THIS FUNCTION

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //Should go seperate?
	glBufferData(GL_ARRAY_BUFFER, bufferSize_B, data, GL_STATIC_DRAW);
		
		
	//Depends on the layout in the shader
	//glEnableVertexAttribArray(0);
	/*glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);*/
}


void IndexBuffer::CreateBuffer(GLsizeiptr bufferSize_B, const void* data)
{
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer); //Should go seperate?
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize_B, data, GL_STATIC_DRAW);


	//Depends on the layout in the shader
	//glEnableVertexAttribArray(0);
	/*glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);*/
}
