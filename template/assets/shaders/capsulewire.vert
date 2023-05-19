#version 330 core

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

const int NUM_SEGMENTS=100;

uniform float radius;
uniform float height;

void main()
{
    vec3 lineVerts[8]=vec3[](
        vec3(-radius,-height/2.f,.0f),
        vec3(-radius,height/2.f,.0f),
        vec3(radius,-height/2.f,.0f),
        vec3(radius,height/2.f,.0f),
        vec3(.0f,-height/2.f,-radius),
        vec3(.0f,height/2.f,-radius),
        vec3(.0f,-height/2.f,radius),
        vec3(.0f,height/2.f,radius)
    );
    const float PI = 3.14159265359;

    float angle=float((gl_VertexID+1%(2*NUM_SEGMENTS))/2)/float(NUM_SEGMENTS);
    angle = angle*2*PI;

    vec3 yAxis=float(gl_VertexID<2*NUM_SEGMENTS)*vec3(radius*cos(angle),-height/2.f,radius*sin(angle))+
        float(gl_VertexID>=6*NUM_SEGMENTS&&gl_VertexID<8*NUM_SEGMENTS)*vec3(radius*cos(angle),height/2.f,radius*sin(angle));

    vec3 xAxis=float(gl_VertexID>=2*NUM_SEGMENTS&&gl_VertexID<4*NUM_SEGMENTS)*vec3(.0f,radius*cos(angle)-height/2.f,radius*sin(angle))+
        float(gl_VertexID>=8*NUM_SEGMENTS&&gl_VertexID<10*NUM_SEGMENTS)*vec3(.0f,radius*cos(angle)+height/2.f,radius*sin(angle));
    
    vec3 zAxis=float(gl_VertexID>=4*NUM_SEGMENTS&&gl_VertexID<6*NUM_SEGMENTS)*vec3(radius*cos(angle),radius*sin(angle)-height/2.f,-.0f)+
        float(gl_VertexID>=10*NUM_SEGMENTS&&gl_VertexID<12*NUM_SEGMENTS)*vec3(radius*cos(angle),radius*sin(angle)+height/2.f,.0f);
    
    vec3 lines=float(gl_VertexID>=12*NUM_SEGMENTS)*lineVerts[max(gl_VertexID,12*NUM_SEGMENTS)-12*NUM_SEGMENTS];

    gl_Position = projection * view * model * vec4(yAxis+xAxis+zAxis+lines, 1.0);
}
