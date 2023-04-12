#version 330 core

in vec3 vsCol;

out vec4 fsColour;

void main()
{
    fsColour = vec4(vsCol, 1.0f);
}
