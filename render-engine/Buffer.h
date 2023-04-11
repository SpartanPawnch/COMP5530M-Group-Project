#pragma once


enum VertexBufferType
{
	PositionsBuffer = 0,
	TextureCoordsBuffer = 1,
	NormalsBuffer = 2,
	ColorsBuffer = 3,
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