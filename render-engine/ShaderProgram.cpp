#include "ShaderProgram.h";

const char* ShaderProgram::readShaderCode(const char* shaderPath)
{
	std::string shaderCode;
	std::ifstream shaderFile;

	shaderFile.exceptions(std::ifstream::failbit |
		std::ifstream::badbit);

	try {
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << " ERROR::SHADER:: " <<
			shaderPath
			<<" ::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}

	return shaderCode.c_str();
}

ShaderProgram::ShaderProgram(const char* vertexPath,
	const char* fragmentPath,
	const char* geometryPath,
	const char* computePath,
	const char* tessControlShader,
	const char* tessEvalShader)
{
	//Load and compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderCode = readShaderCode(vertexPath);
	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
	glCompileShader(vertexShader);

	//Load and compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderCode = readShaderCode(fragmentPath);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShader);

	//Optional shaders
	GLuint tesselationControlShaderID = 0;
	GLuint tesselationEvalShaderID = 0;
	GLuint geometryShaderID = 0;
	GLuint computeShader = 0;

	//Link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//Delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}