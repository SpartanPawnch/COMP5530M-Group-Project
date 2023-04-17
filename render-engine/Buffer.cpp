#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>


#include "Buffer.h"




VertexBuffer::VertexBuffer(unsigned int nVertices, const void* data, VertexBufferType bufferType)
{

	// VAO probably with shaders / or in fn thT CALLS THIS FUNCTION

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //Should go seperate?
	if (bufferType == TexturedObjectBuffer)
	{
		glBufferData(GL_ARRAY_BUFFER, nVertices*sizeof(Vertex), data, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::texCoords));

	}
		
	//else if (bufferType == ColoredObjectBuffer)
	//	glBufferData(GL_ARRAY_BUFFER, nVertices * 9* sizeof(float), data, GL_STATIC_DRAW);
	////
	//unsigned int elementSize = (bufferType == TextureCoordsBuffer) ? 2 : 3;
	/*glVertexAttribPointer(bufferType, elementSize, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(bufferType);*/
}


IndexBuffer::IndexBuffer(GLsizeiptr bufferSize_B, const void* data)
{
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer); //Should go seperate?
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize_B, data, GL_STATIC_DRAW);

}