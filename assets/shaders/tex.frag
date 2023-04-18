#version 330 core

in vec2 vsTex;
in vec3 vsNormal;

out vec4 fsColour;

void main()
{
    vec3 norm;
    if(vsNormal.x < 0.0)
    {
        norm.x = (-1) * vsNormal.x;
    }
    else
    {
        norm.x = vsNormal.x;
    }

    if(vsNormal.y < 0.0)
    {
        norm.y = (-1) * vsNormal.y;
    }
    else
    {
        norm.y = vsNormal.y;
    }

    if(vsNormal.z < 0.0)
    {
        norm.z = (-1) * vsNormal.z;
    }
    else
    {
        norm.z = vsNormal.z;
    }

    //fsColour = vec4(vsTex.x, vsTex.y, 1.0f, 1.0f);
    fsColour = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}
