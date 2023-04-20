#pragma once
#include "../src//model_import/mesh.h"

enum VertexBufferType
{
	Unassigned = 0,
	ColoredObjectBuffer = 1,
	TexturedObjectBuffer = 2
};

class VertexBuffer
{
private:
	GLuint vertexBuffer;
	VertexBufferType type;

public:
	VertexBuffer();
	VertexBuffer(VertexBufferType aType);
	VertexBuffer(GLsizeiptr bufferSize_B, const void *data, VertexBufferType bufferType);


};

class IndexBuffer
{
private:
	GLuint indexBuffer;

public:
	IndexBuffer() {};
	IndexBuffer(GLsizeiptr bufferSize_B, const void* data);

};