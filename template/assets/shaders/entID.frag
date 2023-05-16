#version 330 core

out vec4 fsColour;

uniform vec3 entID;

void main()
{
	fsColour = vec4(entID,1.0);
}
