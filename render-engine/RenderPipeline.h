#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/glew.h>



class RenderPipeline
{

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

	RenderPipeline(const char* vertexPath,
		const char* fragmentPath,
		const char* geometryPath = nullptr,
		const char* computePath = nullptr,
		const char* tessControlPath = nullptr,
		const char* tessEvalPath = nullptr);

	GLuint getProgram();

	~RenderPipeline();

	void setAttributeLayouts();
};