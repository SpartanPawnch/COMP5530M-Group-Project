#pragma once
#include "../src//model_import/mesh.h"

enum VertexBufferType
{
	ColoredObjectBuffer = 0,
	TexturedObjectBuffer = 1
};

class VertexBuffer
{
private:
	GLuint vertexBuffer;

public:
	VertexBuffer(GLsizeiptr bufferSize_B, const void* data, VertexBufferType bufferType);


};

class IndexBuffer
{
private:
	GLuint indexBuffer;

public:
	IndexBuffer(GLsizeiptr bufferSize_B, const void* data);


};