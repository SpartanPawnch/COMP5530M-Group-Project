#version 330 core

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex;
layout (location = 3) in int boneIds[MAX_BONE_INFLUENCE];
layout (location = 4) in float weights[MAX_BONE_INFLUENCE];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 bonesMatrix[MAX_BONES];

out vec2 vsTex;
out vec3 vsNormal;

void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = bonesMatrix[boneIds[i]] * vec4(pos,1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(bonesMatrix[boneIds[i]]) * normal;
        vsNormal += localNormal * weights[i];
    }
		
    mat4 viewModel = view * model;
    gl_Position =  projection * viewModel * totalPosition;
    vsTex = tex;
    vsNormal = normalize(vsNormal);
}