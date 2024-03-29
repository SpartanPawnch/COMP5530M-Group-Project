#include "RenderPipeline.h"

RenderPipeline::RenderPipeline() {
    // lightUniforms.resize(MAX_LIGHTS);
}

RenderPipeline::RenderPipeline(const char* vertexPath, const char* fragmentPath,
    const char* geometryPath, const char* computePath, const char* tessControlPath,
    const char* tessEvalPath) {
    RenderPipeline::createProgram(vertexPath, fragmentPath, geometryPath, computePath,
        tessControlPath, tessEvalPath);

    this->initialised = true;
    VAOs = {};
}

// adapted from COMP5812M Foundations of Modelling and Rendering Coursework 3
bool RenderPipeline::readAndCompileShader(const char* shaderPath, const GLuint& id) {
    std::string shaderCode;
    std::ifstream shaderFile;

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        shaderFile.open(shaderPath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << " ERROR::SHADER:: " << shaderPath << " ::FILE_NOT_SUCCESSFULLY_READ"
            << std::endl;
    }

    char const* sCode = shaderCode.c_str();
    glShaderSource(id, 1, &sCode, NULL);
    glCompileShader(id);

    GLint res = GL_FALSE;
    int infoLogLength;

    // check Shader
    glGetShaderiv(id, GL_COMPILE_STATUS, &res);
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(id, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
        std::cout << &vertexShaderErrorMessage[0] << std::endl;
    }

    std::cout << "Compilation of Shader: " << shaderPath << " "
        << (res == GL_TRUE ? "Success" : "Failed!") << std::endl;

    return res == 1;
}

void RenderPipeline::createProgram(const char* vertexPath, const char* fragmentPath,
    const char* geometryPath, const char* computePath, const char* tessControlPath,
    const char* tessEvalPath) {
    // Load and compile vertex shader
    this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    readAndCompileShader(vertexPath, this->vertexShader);

    // Load and compile fragment shader
    this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    readAndCompileShader(fragmentPath, this->fragmentShader);

    // if tesselation shaders are used
    if (tessControlPath && tessEvalPath) {
        this->tescShader = glCreateShader(GL_TESS_CONTROL_SHADER);
        readAndCompileShader(tessControlPath, this->tescShader);

        this->teseShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
        readAndCompileShader(tessEvalPath, this->teseShader);
    }

    if (geometryPath) {
        this->geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        readAndCompileShader(geometryPath, this->geometryShader);
    }

    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, this->vertexShader);
    glAttachShader(shaderProgram, this->fragmentShader);

    if (tessControlPath && tessEvalPath) {
        glAttachShader(shaderProgram, tescShader);
        glAttachShader(shaderProgram, teseShader);
    }

    if (geometryPath) {
        glAttachShader(shaderProgram, geometryShader);
    }

    glLinkProgram(shaderProgram);

    // Delete shaders
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (geometryShader != 0) {
        glDeleteShader(geometryShader);
    }

    // check shader program for errors
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

RenderPipeline::~RenderPipeline() {
}

GLuint RenderPipeline::getProgram() {
    return this->shaderProgram;
}
GLuint RenderPipeline::getVAO(unsigned int index) {
    return this->VAOs[index];
}
unsigned int RenderPipeline::getNoOfMeshes() {
    return this->VAOs.size();
}
GLuint RenderPipeline::getModelID() {
    return this->ModelID;
}
GLuint RenderPipeline::getViewID() {
    return this->ViewID;
}
GLuint RenderPipeline::getProjectionID() {
    return this->ProjectionID;
}
GLuint RenderPipeline::getLightPosID(int index) {
    return this->lightUniforms[index].lightPosID;
}
GLuint RenderPipeline::getLightAmbientID(int index) {
    return this->lightUniforms[index].lightAmbientID;
}
GLuint RenderPipeline::getLightDiffuseID(int index) {
    return this->lightUniforms[index].lightDiffuseID;
}
GLuint RenderPipeline::getLightSpecularID(int index) {
    return this->lightUniforms[index].lightSpecularID;
}
GLuint RenderPipeline::getDirectionalLightPosID(int index) {
    return this->directionalLightUniforms[index].lightPosID;
}
GLuint RenderPipeline::getDirectionalLightAmbientID(int index) {
    return this->directionalLightUniforms[index].lightAmbientID;
}
GLuint RenderPipeline::getDirectionalLightDiffuseID(int index) {
    return this->directionalLightUniforms[index].lightDiffuseID;
}
GLuint RenderPipeline::getDirectionalLightSpecularID(int index) {
    return this->directionalLightUniforms[index].lightSpecularID;
}
GLuint RenderPipeline::getViewPosID() {
    return this->viewPosID;
}
GLuint RenderPipeline::getGammaID() {
    return this->gammaID;
}
GLuint RenderPipeline::getNumLightsID() {
    return this->numLightsID;
}
GLuint RenderPipeline::getNumDirectionalLightsID() {
    return this->numDirectionalLightsID;
}
GLuint RenderPipeline::getBaseColorID() {
    return this->baseColorID;
}
GLuint RenderPipeline::getEmissiveColorID() {
    return this->emissiveColorID;
}
GLuint RenderPipeline::getRoughnessID() {
    return this->roughnessID;
}
GLuint RenderPipeline::getMetalnessID() {
    return this->metalnessID;
}
GLuint RenderPipeline::getOcclusionID() {
    return this->occlusionID;
}
GLuint RenderPipeline::getbaseColorSamplerLocationID() {
    return this->baseColorSamplerLocation;
}
GLuint RenderPipeline::getroughnessSamplerLocationID() {
    return this->roughnessSamplerLocation;
}

GLuint RenderPipeline::getmetalnessSamplerLocationID() {
    return this->metalnessSamplerLocation;
}

GLuint RenderPipeline::getnormalSamplerLocationID() {
    return this->normalSamplerLocation;
}

GLuint RenderPipeline::getalphaSamplerLocationID() {
    return this->alphaSamplerLocation;
}

GLuint RenderPipeline::getemissiveSamplerLocationID() {
    return this->emissiveSamplerLocation;
}

GLuint RenderPipeline::getocclusionSamplerLocationID() {
    return this->occlusionSamplerLocation;
}

GLuint RenderPipeline::getBonesMatrix() {
    return this->bonesMatrix;
}
GLuint RenderPipeline::getEntID() {
    return this->entID;
}

void RenderPipeline::setUniformLocations() {
    this->ModelID = glGetUniformLocation(this->shaderProgram, "model");
    this->ViewID = glGetUniformLocation(this->shaderProgram, "view");
    this->ProjectionID = glGetUniformLocation(this->shaderProgram, "projection");
    // this->lightPosID = glGetUniformLocation(this->shaderProgram, "lightPos");
    // this->lightColID = glGetUniformLocation(this->shaderProgram, "lightCol");
    this->lightPosID = glGetUniformLocation(this->shaderProgram, "lightPos");
    this->lightColID = glGetUniformLocation(this->shaderProgram, "lightCol");
    this->bonesMatrix = glGetUniformLocation(this->shaderProgram, "bonesMatrix");
}
void RenderPipeline::setIDUniformLocations() {
    setUniformLocations();
    this->entID = glGetUniformLocation(this->shaderProgram, "entID");
}

void RenderPipeline::addVAO(GLuint VAO) {
    VAOs.push_back(VAO);
}

void RenderPipeline::setColourUniformLocations() {
    this->ModelID = glGetUniformLocation(this->shaderProgram, "model");
    this->ViewID = glGetUniformLocation(this->shaderProgram, "view");
    this->ProjectionID = glGetUniformLocation(this->shaderProgram, "projection");
    this->viewPosID = glGetUniformLocation(this->shaderProgram, "viewPos");
    this->gammaID = glGetUniformLocation(this->shaderProgram, "gamma");
    this->numLightsID = glGetUniformLocation(this->shaderProgram, "numLights");

    // light vector
    for (int i = 0; i < MAX_LIGHTS; i++) {
        std::string base = "lights[" + std::to_string(i) + "].";
        this->lightUniforms[i].lightPosID =
            glGetUniformLocation(this->shaderProgram, (base + "position").c_str());
        this->lightUniforms[i].lightAmbientID =
            glGetUniformLocation(this->shaderProgram, (base + "ambient").c_str());
        this->lightUniforms[i].lightDiffuseID =
            glGetUniformLocation(this->shaderProgram, (base + "diffuse").c_str());
        this->lightUniforms[i].lightSpecularID =
            glGetUniformLocation(this->shaderProgram, (base + "specular").c_str());
    }
}

void RenderPipeline::setTextureUniformLocations() {
    this->ModelID = glGetUniformLocation(this->shaderProgram, "model");
    this->ViewID = glGetUniformLocation(this->shaderProgram, "view");
    this->ProjectionID = glGetUniformLocation(this->shaderProgram, "projection");
    this->viewPosID = glGetUniformLocation(this->shaderProgram, "viewPos");
    this->gammaID = glGetUniformLocation(this->shaderProgram, "gamma");
    this->numLightsID = glGetUniformLocation(this->shaderProgram, "numLights");
    this->numDirectionalLightsID = glGetUniformLocation(this->shaderProgram, "numDirectionalLights");
    this->baseColorSamplerLocation = glGetUniformLocation(this->shaderProgram, "baseColorSampler");
    this->roughnessSamplerLocation = glGetUniformLocation(this->shaderProgram, "roughnessSampler");
    this->metalnessSamplerLocation = glGetUniformLocation(this->shaderProgram, "metalnessSampler");
    this->normalSamplerLocation = glGetUniformLocation(this->shaderProgram, "normalSampler");
    this->alphaSamplerLocation = glGetUniformLocation(this->shaderProgram, "alphaSampler");
    this->emissiveSamplerLocation = glGetUniformLocation(this->shaderProgram, "emissiveSampler");
    this->occlusionSamplerLocation = glGetUniformLocation(this->shaderProgram, "occlusionSampler");

    this->baseColorID = glGetUniformLocation(this->shaderProgram, "baseColor");
    this->emissiveColorID = glGetUniformLocation(this->shaderProgram, "emissiveColor");
    
    this->roughnessID = glGetUniformLocation(this->shaderProgram, "roughness");
    this->metalnessID = glGetUniformLocation(this->shaderProgram, "metalness");
    this->occlusionID = glGetUniformLocation(this->shaderProgram, "occlusion");

    this->baseColorSamplerLocation;

    // light vector
    for (int i = 0; i < MAX_LIGHTS; i++) {
        std::string base = "lights[" + std::to_string(i) + "].";
        this->lightUniforms[i].lightPosID =
            glGetUniformLocation(this->shaderProgram, (base + "position").c_str());
        this->lightUniforms[i].lightAmbientID =
            glGetUniformLocation(this->shaderProgram, (base + "ambient").c_str());
        this->lightUniforms[i].lightDiffuseID =
            glGetUniformLocation(this->shaderProgram, (base + "diffuse").c_str());
        this->lightUniforms[i].lightSpecularID =
            glGetUniformLocation(this->shaderProgram, (base + "specular").c_str());
    }

    // directional lights vector
    for (int i = 0; i < MAX_LIGHTS; i++) {
        std::string base = "directionalLights[" + std::to_string(i) + "].";
        this->directionalLightUniforms[i].lightPosID =
            glGetUniformLocation(this->shaderProgram, (base + "position").c_str());
        this->directionalLightUniforms[i].lightAmbientID =
            glGetUniformLocation(this->shaderProgram, (base + "ambient").c_str());
        this->directionalLightUniforms[i].lightDiffuseID =
            glGetUniformLocation(this->shaderProgram, (base + "diffuse").c_str());
        this->directionalLightUniforms[i].lightSpecularID =
            glGetUniformLocation(this->shaderProgram, (base + "specular").c_str());
    }
}
