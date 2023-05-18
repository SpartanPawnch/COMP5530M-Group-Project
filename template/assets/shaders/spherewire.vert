#version 330 core

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

const int NUM_SEGMENTS=100;

void main()
{
    const float PI = 3.14159265359;

    float angle=float((gl_VertexID+1%(2*NUM_SEGMENTS))/2)/float(NUM_SEGMENTS);
    angle = angle*2*PI;

    vec3 yAxis=float(gl_VertexID<2*NUM_SEGMENTS)*vec3(cos(angle),.0f,sin(angle));
    vec3 xAxis=float(gl_VertexID>=2*NUM_SEGMENTS&&gl_VertexID<4*NUM_SEGMENTS)*vec3(.0f,cos(angle),sin(angle));
    vec3 zAxis=float(gl_VertexID>=4*NUM_SEGMENTS)*vec3(cos(angle),sin(angle),.0f);

    gl_Position = projection * view * model * vec4(yAxis+xAxis+zAxis, 1.0);
}
