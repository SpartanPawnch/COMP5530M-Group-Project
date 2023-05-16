#version 440

uniform mat4 previewInverse;
uniform mat4 MVP;

void main(){
    const vec3 coords[16]=vec3[](
        vec3(-1.f,1.f,1.0f),vec3(1.f,1.f,1.0f),
        vec3(-1.f,-1.f,1.0f),vec3(1.f,-1.f,1.0f),
        vec3(-1.f,1.f,1.0f),vec3(-1.f,-1.f,1.0f),
        vec3(1.f,1.f,1.0f),vec3(1.f,-1.f,1.0f),
        vec3(-1.f,1.f,.0f),vec3(-1.f,1.f,1.0f),
        vec3(1.f,1.f,.0f),vec3(1.f,1.f,1.0f),
        vec3(-1.f,-1.f,.0f),vec3(-1.f,-1.f,1.0f),
        vec3(1.f,-1.f,.0f),vec3(1.f,-1.f,1.0f)
    );

    gl_Position = MVP*previewInverse* vec4(
        coords[gl_VertexID],
        1.0f
    );
}
