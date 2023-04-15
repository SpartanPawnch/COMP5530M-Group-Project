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

	GLuint ModelID;
	GLuint ViewID;
	GLuint ProjectionID;
	GLuint lightPosID;
	GLuint lightColID;

	GLuint VAO;
	bool readAndCompileShader(const char* shaderPath, const GLuint& id);
	void createProgram(const char* vertexPath,
		const char* fragmentPath,
		const char* geometryPath,
		const char* computePath,
		const char* tessControlPath,
		const char* tessEvalPath);

public:

	RenderPipeline(const char* vertexPath,
		const char* fragmentPath,
		const char* geometryPath = nullptr,
		const char* computePath = nullptr,
		const char* tessControlPath = nullptr,
		const char* tessEvalPath = nullptr);

	GLuint getProgram();
	GLuint getVAO();
	GLuint getModelID();
	GLuint getViewID();
	GLuint getProjectionID();
	GLuint getLightPosID();
	GLuint getLightColID();

	~RenderPipeline();

	void setAttributeLayouts();

	void setUniformLocations();
	
};