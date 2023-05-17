#version 440

uniform mat4 modelView;
uniform mat4 proj;

void main(){
    vec3 offset=vec3(
        float(gl_VertexID<2)*(-1.f+2.f*gl_VertexID),
        float(gl_VertexID>=2&&gl_VertexID<4)*(-1.f+2.f*(gl_VertexID-2)),
        float(gl_VertexID>=4)*(-1.f+2.f*(gl_VertexID-4))
    );
    
    gl_Position=proj*modelView*vec4(offset,1.f);
}
