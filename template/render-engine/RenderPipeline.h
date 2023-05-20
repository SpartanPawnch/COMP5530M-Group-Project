#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/glew.h>

const int MAX_LIGHTS = 20;

class RenderPipeline {
    struct LightUniform {
        GLuint lightPosID;

        GLuint lightAmbientID;
        GLuint lightDiffuseID;
        GLuint lightSpecularID;
    };

private:
    GLuint shaderProgram;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint computeShader;
    GLuint tescShader;
    GLuint teseShader;
    GLuint geometryShader;

public:
    GLuint ModelID;
    GLuint ViewID;
    GLuint ProjectionID;
    LightUniform lightUniforms[MAX_LIGHTS];
    LightUniform directionalLightUniforms[MAX_LIGHTS];
    GLuint viewPosID;
    GLuint gammaID;
    GLuint numLightsID;
    GLuint numDirectionalLightsID;
    GLuint lightPosID;
    GLuint lightColID;
    GLuint baseColorID;
    GLuint emissiveColorID;
    GLuint roughnessID;
    GLuint metalnessID;
    GLuint occlusionID;
    GLuint bonesMatrix;
    GLuint baseColorSamplerLocation;
    GLuint roughnessSamplerLocation;
    GLuint metalnessSamplerLocation;
    GLuint normalSamplerLocation;
    GLuint alphaSamplerLocation;
    GLuint emissiveSamplerLocation;
    GLuint occlusionSamplerLocation;
    GLuint entID;

    std::vector<GLuint> VAOs;

    GLuint VAO;

    bool readAndCompileShader(const char* shaderPath, const GLuint& id);
    void createProgram(const char* vertexPath, const char* fragmentPath, const char* geometryPath,
        const char* computePath, const char* tessControlPath, const char* tessEvalPath);

    bool initialised = false;

    RenderPipeline();

    RenderPipeline(const char* vertexPath, const char* fragmentPath,
        const char* geometryPath = nullptr, const char* computePath = nullptr,
        const char* tessControlPath = nullptr, const char* tessEvalPath = nullptr);

    ~RenderPipeline();

    GLuint getProgram();
    GLuint getVAO(unsigned int index);
    GLuint getModelID();
    GLuint getViewID();
    GLuint getProjectionID();
    GLuint getLightPosID(int index);
    GLuint getLightAmbientID(int index);
    GLuint getLightDiffuseID(int index);
    GLuint getLightSpecularID(int index);
    GLuint getDirectionalLightPosID(int index);
    GLuint getDirectionalLightAmbientID(int index);
    GLuint getDirectionalLightDiffuseID(int index);
    GLuint getDirectionalLightSpecularID(int index);
    GLuint getViewPosID();
    GLuint getGammaID();
    GLuint getNumLightsID();
    GLuint getNumDirectionalLightsID();
    GLuint getBaseColorID();
    GLuint getEmissiveColorID();
    GLuint getRoughnessID();
    GLuint getMetalnessID();
    GLuint getOcclusionID();
    GLuint getbaseColorSamplerLocationID();
    GLuint getroughnessSamplerLocationID();
    GLuint getmetalnessSamplerLocationID();
    GLuint getnormalSamplerLocationID();
    GLuint getalphaSamplerLocationID();
    GLuint getemissiveSamplerLocationID();
    GLuint getocclusionSamplerLocationID();
    GLuint getBonesMatrix();
    GLuint getEntID();

    void setAttributeLayouts();

    void setUniformLocations();
    void setColourUniformLocations();
    void setTextureUniformLocations();
    void setIDUniformLocations();

    unsigned int getNoOfMeshes();
    void addVAO(GLuint VAO);
};
