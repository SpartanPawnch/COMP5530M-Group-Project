#version 440

uniform int size=50;
uniform vec2 offset=vec2(.0f);
uniform mat4 view;
uniform mat4 projection;

void main(){
    int posX=int(gl_VertexID<(size*2+2))*(size*(gl_VertexID%2))+
        int(gl_VertexID>=(size*2+2))*((gl_VertexID-(size*2+2))/2);
    int posY=int(gl_VertexID>=(size*2+2))*(size*(gl_VertexID%2))+
        int(gl_VertexID<(size*2+2))*(gl_VertexID/2);
    gl_Position = projection*view * vec4(
        5.f*(posX-size/2+offset.x),
        .0f,
        5.f*(posY-size/2+offset.y),
        1.0f
    );
}
e=50;
uniform vec2 offset=vec2(.0f);
uniform mat4 view;
uniform mat4 projection;

void main(){
    int posX=int(gl_VertexID<(size*2+2))*(size*(gl_VertexID%2))+
        int(gl_VertexID>=(size*2+2))*((gl_VertexID-(size*2+2))/2);
    int posY=int(gl_VertexID>=(size*2+2))*(size*(gl_VertexID%2))+
        int(gl_VertexID<(size*2+2))*(gl_VertexID/2);
    gl_Position = projection*view * vec4(
        5.f*(posX-size/2+offset.x),
        .0f,
        5.f*(posY-size/2+offset