#version 330 core

in vec3 vsPos;
in vec3 vsCol;
in vec3 vsNormal;

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

uniform Light light;
uniform float gamma;
uniform vec3 viewPos;

void main()
{
   Material material;
    material.diffuse = vec3(0.2, 0.2, 0.2);
    material.diffuse = vec3(0.8, 0.8, 0.8);
    material.specular = vec3(0.2, 0.2, 0.2);
    material.shininess = 16.0;

   vec3 colour = vsCol;
   //using normal instead of color
   //ambient
   vec3 ambient = light.ambient * material.ambient;

    //diffuse
    vec3 lightDir = normalize(light.position - vsPos);
    vec3 normal = normalize(vsNormal);
    float diff = clamp(dot(lightDir, vsNormal), 0, 1);
    vec3 diffuse = light.diffuse * diff *  material.diffuse;

    //specular
    vec3 viewDir = normalize(viewPos - vsPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * material.specular; 


    fsColour = vec4(colour,1.f) *
            (vec4(ambient, 1.f) + vec4(diffuse, 1.f) + vec4(specular, 1.f));
    fsColour.rgb = pow(fsColour.rgb, vec3(1.0/gamma));
}
