#version 330 core

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

vec3 cubeVertices[8] = vec3[](
        vec3(-1.0f, -1.0f, -1.0f),
        vec3( 1.0f, -1.0f, -1.0f),
        vec3( 1.0f,  1.0f, -1.0f),
        vec3(-1.0f,  1.0f, -1.0f),
        vec3(-1.0f, -1.0f,  1.0f),
        vec3( 1.0f, -1.0f,  1.0f),
        vec3( 1.0f,  1.0f,  1.0f),
        vec3(-1.0f,  1.0f,  1.0f)
);

int cubeIndices[24] = int[](
    0, 1,
    1, 2,
    2, 3,
    3, 0,
    4, 5,
    5, 6,
    6, 7,
    7, 4,
    0, 4,
    1, 5,
    2, 6,
    3, 7
);

void main()
{
    int cubeVertexIndex = cubeIndices[gl_VertexID];

    gl_Position = projection * view * model * vec4(cubeVertices[cubeVertexIndex], 1.0);
}
