#version 330 core

uniform sampler2D icon;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec4 color = texture(icon, TexCoords);
    if(color.a<.5f)
        discard;
    //if(color.a < 0.5) discard;
    FragColor = color;
}
