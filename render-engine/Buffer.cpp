#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>


#include "Buffer.h"




VertexBuffer::VertexBuffer( GLsizeiptr bufferSize_B, const void *data, VertexBufferType bufferType)
{


	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //Should go seperate?
	if (bufferType == TexturedObjectBuffer)
	{	//models imported from assimp with struct Vertex format
		glBufferData(GL_ARRAY_BUFFER, bufferSize_B, data, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	}
	else if (bufferType == ColoredObjectBuffer)
	{
		glBufferData(GL_ARRAY_BUFFER, bufferSize_B, data, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); //position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(0));
		glEnableVertexAttribArray(1); //normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2); //colour
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	}

}


IndexBuffer::IndexBuffer(GLsizeiptr bufferSize_B, const void* data)
{
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer); //Should go seperate?
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize_B, data, GL_STATIC_DRAW);

}