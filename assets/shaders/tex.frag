#version 330 core

in vec2 vsTex;
in vec3 vsNormal;

out vec4 fsColour;

uniform sampler2D texSampler;

void main()
{
    //fsColour = vec4(abs(vsNormal), 1.0f);

    fsColour = texture(texSampler,vsTex);
}
