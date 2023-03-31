#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/glew.h>



class ShaderProgram
{
	enum ShaderStage
	{
		VertexShader = 0,
		FragmentShader = 1,
		GeometryShader = 2,
		ComputeShader = 3,
		TessellationControlShader = 4,
		TessellationEvaluationShader = 5
	};

private:
	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint computeShader;
	GLuint tescShader;
	GLuint teseShader;
	GLuint geometryShader;

	bool readAndCompileShader(const char* shaderPath, const GLuint& id);

public:

	ShaderProgram(const char* vertexPath,
		const char* fragmentPath,
		const char* geometryPath = nullptr,
		const char* computePath = nullptr,
		const char* tessControlPath = nullptr,
		const char* tessEvalPath = nullptr);

	GLuint getProgram();

	~ShaderProgram();
};