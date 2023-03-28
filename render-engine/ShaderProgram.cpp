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
	const char* tessControlPath,
	const char* tessEvalPath)
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
	GLuint tesselationControlShader = 0;
	GLuint tesselationEvalShader = 0;
	GLuint geometryShader = 0;
	GLuint computeShader = 0;

	//if tesselation shaders are used
	if (tessControlPath && tessEvalPath)
	{
		tesselationControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
		const char* tescCode = readShaderCode(tessControlPath);
		glShaderSource(tesselationControlShader, 1, &tescCode, NULL);
		glCompileShader(tesselationControlShader);

		tesselationEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		const char* teseCode = readShaderCode(tessEvalPath);
		glShaderSource(tesselationEvalShader, 1, &teseCode, NULL);
		glCompileShader(tesselationEvalShader);
	}

	if (geometryPath)
	{
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		const char* geoCode = readShaderCode(geometryPath);
		glShaderSource(geometryShader, 1, &geoCode, NULL);
		glCompileShader(geometryShader);
	}



	//Link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	
	if (tessControlPath && tessEvalPath)
	{
		glAttachShader(shaderProgram, tesselationControlShader);
		glAttachShader(shaderProgram, tesselationEvalShader);
	}

	if (geometryPath)
	{
		glAttachShader(shaderProgram, geometryShader);
	}

	glLinkProgram(shaderProgram);

	//Delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	if (tesselationControlShader != 0 && tesselationEvalShader != 0)
	{
		glDeleteShader(tesselationControlShader);
		glDeleteShader(tesselationEvalShader);
	}

	if (geometryShader != 0)
	{
		glDeleteShader(geometryShader);
	}
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(this->shaderProgram);
}