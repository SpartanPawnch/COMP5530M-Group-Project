#version 440

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_tex_coordinate;
in vec3 vs_normal;

out vec4 fs_color;

uniform sampler2D grassTexture;

uniform vec3 lightPos0;
uniform vec3 cameraPos;

void main()
{
    //fs_color = vec4(vs_color, 1.f);

    //Ambient Light
    vec3 ambientLight = vec3(1.f, 1.f, 1.f);
    float ambientStrength = 0.3;
    ambientLight = ambientLight * ambientStrength;

    //Diffuse Light
    vec3 posToLightDirVec = normalize(lightPos0 - vs_position);
    vec3 diffuseColor = vec3(1.f,1.f,1.f);
    float diffuse = clamp(dot(posToLightDirVec, vs_normal), 0, 1);
    vec3 diffuseFinal = diffuseColor * diffuse;

    //Specular lightPos0
    vec3 lightToPosDirVec = normalize(vs_position - lightPos0);
    vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal)));
    vec3 posToViewDirVec = normalize(cameraPos - vs_position);
    float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), 16);
    vec3 specularFinal = vec3(1.f, 1.f, 1.f) * specularConstant;


    fs_color = vec4(texture(grassTexture, vs_tex_coordinate).xyz,1.f) *
            (vec4(ambientLight, 1.f) + vec4(diffuseFinal, 1.f) + vec4(specularFinal,1.f));
}
