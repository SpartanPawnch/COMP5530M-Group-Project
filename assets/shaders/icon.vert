#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;

//icon positions
const vec3 iconPositions[4] = {
    vec3(-0.5f,  0.5f,  0.0f),
    vec3(0.5f,  0.5f,  0.0f),
    vec3(-0.5f, -0.5f,  0.0f),
    vec3(0.5f, -0.5f,  0.0f)
};

const vec2 iconTexCoords[4] = {
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(0.0f, 1.0f),
    vec2(1.0f, 1.0f)
};

const int iconIndices[6] = {
    0, 1, 2,
    1, 2, 3
};

void main()
{
    TexCoords = iconTexCoords[iconIndices[gl_VertexID]];
    gl_Position = projection * view * model * vec4(iconPositions[iconIndices[gl_VertexID]], 1.0);
}



