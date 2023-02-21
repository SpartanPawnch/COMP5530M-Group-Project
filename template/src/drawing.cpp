#include<GL/glew.h>
#include<glm/vec3.hpp>
#include<stdio.h>
static GLuint VBO;
static GLuint VAO;

static GLuint shader;

void initGraphics() {
    //create buffer
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexArrayAttrib(VAO, 0);
    const char* vert = R"(
        #version 330
        layout(location=0) in vec2 pos;
        uniform float aspect=16.0/9.0;
        uniform float scale=.2;
        uniform vec2 move=vec2(.0,-3.0);
        void main() {
            vec2 worldPos=scale*(pos+move);
            worldPos.x=worldPos.x/aspect;
            gl_Position = vec4(worldPos.x, worldPos.y, .0, 1.0);
        }
    )";

    //create shader
    int success;
    char glErrInfo[512];

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vert, NULL);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertShader, 512, NULL, glErrInfo);
        printf("Vertex Shader Error:%s", glErrInfo);
    }

    const char* frag = R"(
        #version 330
        out vec4 fragColor;
        uniform vec4 color=vec4(.0,.1,.8,.5);
        void main(){
            fragColor=color;
        }
    )";

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &frag, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, glErrInfo);
        printf("Fragment Shader Error:%s", glErrInfo);
    }

    shader = glCreateProgram();
    glAttachShader(shader, vertShader);
    glAttachShader(shader, fragShader);
    glLinkProgram(shader);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    //GL settings
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
}