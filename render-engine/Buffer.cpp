#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>


#include "Buffer.h"


void VertexBuffer::CreateBuffer(GLsizeiptr bufferSize_B, const void* data)
{		

	// VAO probably with shaders / or in fn thT CALLS THIS FUNCTION

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer); //Should go seperate?
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


class IndexBuffer
{
	void CreateBuffer(glm::vec3* vertices)
	{
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer); //Should go seperate?
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);


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
};