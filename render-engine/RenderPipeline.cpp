#include "RenderPipeline.h";


RenderPipeline::RenderPipeline(const char* vertexPath,
	const char* fragmentPath,
	const char* geometryPath,
	const char* computePath,
	const char* tessControlPath,
	const char* tessEvalPath)
{
	RenderPipeline::createProgram(vertexPath, fragmentPath, geometryPath,
		computePath, tessControlPath, tessEvalPath);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

		
}
//adapted from COMP5812M Foundations of Modelling and Rendering Coursework 3
bool RenderPipeline::readAndCompileShader(const char* shaderPath, const GLuint& id)
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
	
	
	char const* sCode = shaderCode.c_str();
	glShaderSource(id, 1, &sCode, NULL);
	glCompileShader(id);

	GLint res = GL_FALSE;
	int infoLogLength;

	//check Shader
	glGetShaderiv(id, GL_COMPILE_STATUS, &res);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(id, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
		std::cout << &vertexShaderErrorMessage[0] << std::endl;
	}

	std::cout << "Compilation of Shader: " << shaderPath << " " <<
		(res == GL_TRUE ? "Success" : "Failed!") << std::endl;

	return res == 1;
	
}


void RenderPipeline::createProgram(const char* vertexPath,
	const char* fragmentPath,
	const char* geometryPath,
	const char* computePath,
	const char* tessControlPath,
	const char* tessEvalPath)
{
	//Load and compile vertex shader
	this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
	readAndCompileShader(vertexPath, this->vertexShader);

	//Load and compile fragment shader
	this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	readAndCompileShader(fragmentPath, this->fragmentShader);

	//Optional shaders
	this->tescShader = 0;
	this->teseShader = 0;
	this->geometryShader = 0;
	this->computeShader = 0;

	//if tesselation shaders are used
	if (tessControlPath && tessEvalPath)
	{
		this->tescShader = glCreateShader(GL_TESS_CONTROL_SHADER);
		readAndCompileShader(tessControlPath, this->tescShader);

		this->teseShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		readAndCompileShader(tessEvalPath, this->teseShader);
	}

	if (geometryPath)
	{
		this->geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		readAndCompileShader(geometryPath, this->geometryShader);
	}

	//Link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, this->vertexShader);
	glAttachShader(shaderProgram, this->fragmentShader);
	
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
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
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

RenderPipeline::~RenderPipeline(){	}

GLuint RenderPipeline::getProgram(){ return this->shaderProgram;}
GLuint RenderPipeline::getVAO(){ return this->VAO;}
GLuint RenderPipeline::getModelID(){ return this->ModelID;}
GLuint RenderPipeline::getViewID(){ return this->ViewID;}
GLuint RenderPipeline::getProjectionID(){ return this->ProjectionID;}
GLuint RenderPipeline::getLightPosID(){ return this->lightPosID;}
GLuint RenderPipeline::getLightColID(){	return this->lightColID;}

void RenderPipeline::setUniformLocations()
{
	//GLuint ModelID = glGetUniformLocation(getPipeline(ColorPipeline)->getProgram(), "model");
	//GLuint ViewID = glGetUniformLocation(getPipeline(ColorPipeline)->getProgram(), "view");
	//GLuint ProjectionID = glGetUniformLocation(getPipeline(ColorPipeline)->getProgram(), "projection");

	this->ModelID = glGetUniformLocation(this->shaderProgram, "model");
	this->ViewID = glGetUniformLocation(this->shaderProgram, "view");
	this->ProjectionID = glGetUniformLocation(this->shaderProgram, "projection");
	this->lightPosID = glGetUniformLocation(this->shaderProgram, "lightPos");
	this->lightColID = glGetUniformLocation(this->shaderProgram, "lightCol");
}


