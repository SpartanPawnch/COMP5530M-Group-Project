#pragma once



class VertexBuffer
{
private:
	GLuint vertexBuffer;

public:
	VertexBuffer(GLsizeiptr bufferSize_B, const void* data);


};

class IndexBuffer
{
private:
	GLuint indexBuffer;

public:
	IndexBuffer(GLsizeiptr bufferSize_B, const void* data);


};