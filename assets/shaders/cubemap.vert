#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

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

    int cubeIndices[36] = int[](
        0, 1, 2, 0, 2, 3,
        1, 5, 6, 1, 6, 2,
        5, 4, 7, 5, 7, 6,
        4, 0, 3, 4, 3, 7,
        3, 2, 6, 3, 6, 7,
        4, 5, 1, 4, 1, 0
    );

void main()
{
    int cubeVertexIndex = cubeIndices[gl_VertexID];

    TexCoords = cubeVertices[cubeVertexIndex];
    gl_Position = projection * view * vec4(cubeVertices[cubeVertexIndex], 1.0);
}