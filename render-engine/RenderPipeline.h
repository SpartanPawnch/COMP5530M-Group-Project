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

	std::vector <GLuint> VAOs;
	
	bool readAndCompileShader(const char* shaderPath, const GLuint& id);
	void createProgram(const char* vertexPath,
		const char* fragmentPath,
		const char* geometryPath,
		const char* computePath,
		const char* tessControlPath,
		const char* tessEvalPath);

public:

	bool initialised = false;

	RenderPipeline();

	RenderPipeline(const char* vertexPath,
		const char* fragmentPath,
		const char* geometryPath = nullptr,
		const char* computePath = nullptr,
		const char* tessControlPath = nullptr,
		const char* tessEvalPath = nullptr);

	GLuint getProgram();
	GLuint getVAO(unsigned int index);
	GLuint getModelID();
	GLuint getViewID();
	GLuint getProjectionID();
	GLuint getLightPosID();
	GLuint getLightColID();

	~RenderPipeline();

	void setAttributeLayouts();

	void setUniformLocations();

	unsigned int getNoOfMeshes();
	void addVAO(GLuint VAO);
	
};