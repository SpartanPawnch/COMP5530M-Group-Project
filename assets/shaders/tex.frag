#version 330 core

in vec3 vsPos;
in vec3 vsNormal;
in vec2 vsTex;

out vec4 fsColour;

struct Light 
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

const int MAX_LIGHTS = 20;
uniform int numLights;
uniform Light lights[MAX_LIGHTS];

uniform float gamma;
uniform vec3 viewPos;

void main()
{
    Material material;
    material.ambient = vec3(0.2, 0.2, 0.2);
    material.diffuse = vec3(0.8, 0.8, 0.8);
    material.specular = vec3(0.2, 0.2, 0.2);
    material.shininess = 16.0;

    vec3 normal;
    if(vsNormal.x < 0.0)
    {
        normal.x = -vsNormal.x; 
    }
    else{ normal.x = vsNormal.x;
    }

    if(vsNormal.y < 0.0)
    {
        normal.y = -vsNormal.y; 
    }
    else{ normal.y = vsNormal.y;
    }

    if(vsNormal.z < 0.0)
    {
        normal.z = -vsNormal.z; 
    }
    else{ normal.z = vsNormal.z;
    }
    fsColour = vec4(normal, 1.0f);
    vec3 colour = normal;
    vec3 ambient = lights[0].ambient * material.ambient;

    //diffuse
    vec3 lightDir = normalize(lights[0].position - vsPos);
    normal = normalize(vsNormal);
    float diff = clamp(dot(lightDir, vsNormal), 0, 1);
    vec3 diffuse = lights[0].diffuse * diff *  material.diffuse;

    //specular
    vec3 viewDir = normalize(viewPos - vsPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * lights[0].specular * material.specular; 

    fsColour = vec4(colour,1.f) *
            (vec4(ambient, 1.f) + vec4(diffuse, 1.f) + vec4(specular, 1.f));
    fsColour.rgb = pow(fsColour.rgb, vec3(1.0/gamma));
}
