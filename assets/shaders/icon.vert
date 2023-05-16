#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;

//icon positions
const vec3 iconPositions[4] = vec3[](
    vec3(-0.5f,  0.5f,  0.0f),
    vec3(0.5f,  0.5f,  0.0f),
    vec3(-0.5f, -0.5f,  0.0f),
    vec3(0.5f, -0.5f,  0.0f)
);

const vec2 iconTexCoords[4] = vec2[](
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(0.0f, 1.0f),
    vec2(1.0f, 1.0f)
);

const int iconIndices[6] = int[](
    0, 2, 1,
    1, 2, 3
);

void main()
{
    vec3 right=vec3(view[0][0],view[1][0],view[2][0]);
    vec3 up=vec3(view[0][1],view[1][1],view[2][1]);

    TexCoords = iconTexCoords[iconIndices[gl_VertexID]];
    vec3 iconPos=iconPositions[iconIndices[gl_VertexID]];
    gl_Position = projection * view * vec4(right*iconPos.x+vec3(.0f,iconPos.y,.0f)+model[3].xyz, 1.0);
}



