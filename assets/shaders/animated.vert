#version 330 core

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex;
layout (location = 3) in ivec4 boneIds;
layout (location = 4) in vec4 weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 bonesMatrix[MAX_BONES];

out vec3 vsPos;
out vec2 vsTex;
out vec3 vsNormal;

void main()
{
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal=vec3(.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            break;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = bonesMatrix[boneIds[i]] * vec4(pos,1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(bonesMatrix[boneIds[i]]) * normal;
        totalNormal += localNormal * weights[i];
    }
    
    
    vsPos = vec3((model * totalPosition).xyz);
    gl_Position =  projection * view * model * totalPosition;
    vsTex = tex;
    vsNormal = normalize(transpose(inverse(mat3(model)))*totalNormal);
}
