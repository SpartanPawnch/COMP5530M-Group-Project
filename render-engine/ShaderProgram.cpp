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
	std::cout << shaderCode.c_str() << std::endl;
	return shaderCode.c_str();;
}

ShaderProgram::ShaderProgram(const char* vertexPath,
	const char* fragmentPath,
	const char* geometryPath,
	const char* computePath,
	const char* tessControlPath,
	const char* tessEvalPath)
{
	//Load and compile vertex shader
	this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderCode = readShaderCode(vertexPath);
	std::cout << vertexShaderCode << std::endl;
	glShaderSource(this->vertexShader, 1, &vertexShaderCode, NULL);
	glCompileShader(this->vertexShader);

	//Load and compile fragment shader
	this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderCode = readShaderCode(fragmentPath);
	glShaderSource(this->fragmentShader, 1, &fragmentShaderCode, NULL);
	glCompileShader(this->fragmentShader);

	//Optional shaders
	this->tescShader = 0;
	this->teseShader = 0;
	this->geometryShader = 0;
	this->computeShader = 0;

	//if tesselation shaders are used
	if (tessControlPath && tessEvalPath)
	{
		this->tescShader = glCreateShader(GL_TESS_CONTROL_SHADER);
		const char* tescCode = readShaderCode(tessControlPath);
		glShaderSource(this->tescShader, 1, &tescCode, NULL);
		glCompileShader(this->tescShader);

		this->teseShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		const char* teseCode = readShaderCode(tessEvalPath);
		glShaderSource(this->teseShader, 1, &teseCode, NULL);
		glCompileShader(this->teseShader);
	}

	if (geometryPath)
	{
		this->geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
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
		glAttachShader(shaderProgram, tescShader);
		glAttachShader(shaderProgram, teseShader);
	}

	if (geometryPath)
	{
		glAttachShader(shaderProgram, geometryShader);
	}

	glLinkProgram(shaderProgram);

	//Delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	if (tescShader != 0 && teseShader != 0)
	{
		glDeleteShader(tescShader);
		glDeleteShader(teseShader);
	}

	if (geometryShader != 0)
	{
		glDeleteShader(geometryShader);
	}

	// check shader program for errors
	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(this->shaderProgram);
}

GLuint ShaderProgram::getProgram()
{
	return this->shaderProgram;
}