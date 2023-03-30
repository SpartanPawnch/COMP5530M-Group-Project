#version 440

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_tex_coordinate;
layout (location = 3) in vec3 vertex_normal;


out vec3 vs_position;
out vec3 vs_color;
out vec2 vs_tex_coordinate;
out vec3 vs_normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
    vs_position = vec4(ModelMatrix * vec4(vertex_position,1.f)).xyz;
    vs_color = vertex_color;
    vs_tex_coordinate = vec2(vertex_tex_coordinate.x, vertex_tex_coordinate.y * -1.f);
    vs_normal = mat3(ModelMatrix) * vertex_normal;

    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.f);
}
