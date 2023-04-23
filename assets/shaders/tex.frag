#version 330 core

in vec2 vsTex;
in vec3 vsNormal;

out vec4 fsColour;

void main()
{
    fsColour = vec4(abs(vsNormal), 1.0f);
}
