#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 vsPos;
out vec3 vsNormal;
out vec2 vsTex;
out vec3 vsTangent;
out vec3 vsBitangent;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);

    vsPos = vec3(model * vec4(pos, 1.0));
    vsTex = tex;
    vsNormal = mat3(transpose(inverse(model))) * normal;
    vsTangent = mat3(transpose(inverse(model))) * tangent;
    vsBitangent = mat3(transpose(inverse(model))) * bitangent;
}