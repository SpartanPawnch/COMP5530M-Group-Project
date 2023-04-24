#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 col;

out vec3 vsPos;
out vec3 vsCol;
out vec3 vsNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);

    vsPos = vec3(model * vec4(pos, 1.0));
    vsCol = col;
    vsNormal = mat3(transpose(inverse(model))) * normal;
}