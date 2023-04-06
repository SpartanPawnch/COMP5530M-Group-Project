#pragma once



class VertexBuffer
{
	private:
		GLuint vertexbuffer;

	public:
		void CreateBuffer(GLsizeiptr bufferSize_B, const void* data);


};

class IndexBuffer
{
private:
	GLuint vertexbuffer;

public:
	void CreateBuffer(unsigned int bufferSize);


};