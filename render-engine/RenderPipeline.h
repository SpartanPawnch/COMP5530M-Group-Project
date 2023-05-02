#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/glew.h>

class RenderPipeline {

  private:
    GLuint shaderProgram;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint computeShader;
    GLuint tescShader;
    GLuint teseShader;
    GLuint geometryShader;

  public:
    GLuint VAO;
    bool readAndCompileShader(const char* shaderPath, const GLuint& id);
    void createProgram(const char* vertexPath, const char* fragmentPath, const char* geometryPath,
        const char* computePath, const char* tessControlPath, const char* tessEvalPath);

    GLuint ModelID;
    GLuint ViewID;
    GLuint ProjectionID;
    GLuint lightPosID;
    GLuint lightColID;
    GLuint bonesMatrix;
    GLuint entID;

    std::vector<GLuint> VAOs;

    bool initialised = false;

    RenderPipeline();

    RenderPipeline(const char* vertexPath, const char* fragmentPath,
        const char* geometryPath = nullptr, const char* computePath = nullptr,
        const char* tessControlPath = nullptr, const char* tessEvalPath = nullptr);

    GLuint getProgram();
    GLuint getVAO(unsigned int index);
    GLuint getModelID();
    GLuint getViewID();
    GLuint getProjectionID();
    GLuint getLightPosID();
    GLuint getLightColID();
    GLuint getBonesMatrix();
    GLuint getEntID();

    ~RenderPipeline();

    void setAttributeLayouts();

    void setUniformLocations();
    void setIDUniformLocations();

    unsigned int getNoOfMeshes();
    void addVAO(GLuint VAO);
};
