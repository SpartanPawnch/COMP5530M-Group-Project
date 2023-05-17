#version 330 core

uniform sampler2D icon;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec4 color = texture(icon, TexCoords);
    if(color.a<1e-8)
        discard;
    //if(color.a < 0.5) discard;
    FragColor = color;
}
