#pragma once



class VertexBuffer
{
	private:
		GLuint vertexBuffer;

	public:
		void CreateBuffer(GLsizeiptr bufferSize_B, const void* data);


};

class IndexBuffer
{
private:
	GLuint indexBuffer;

public:
	void CreateBuffer(GLsizeiptr bufferSize_B, const void* data);


};