#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>

enum ShaderStage 
{
	VertexShader = 0,
	FragmentShader = 1,
	GeometryShader = 2,
	ComputeShader = 3,
	TessellationControlShader = 4,
	TessellationEvaluationShader = 5
};

class ShaderProgram
{
private:
	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragmentShader;

	const char* readShaderCode(const char* shaderPath);

public:
	ShaderProgram(const char* vertexPath,
		const char* fragmentPath,
		const char* geometryPath = nullptr,
		const char* computePath = nullptr,
		const char* tessControlShader = nullptr,
		const char* tessEvalShader = nullptr);
};