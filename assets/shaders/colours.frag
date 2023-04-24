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

uniform vec3 lightPos;
uniform vec3 lightCol;

uniform Material material;
uniform Light light;

uniform vec3 viewPos;

void main()
{
   vec3 color = vsCol;
   //using normal instead of color
   //ambient
   vec3 ambient = light.ambient * material.ambient *  color;
   //diffuse
   vec3 lightDir = normalize(light.position - vsPos);
   vec3 normal = normalize(vsNormal);
   float diff = max(dot(lightDir, normal), 0.0);
   vec3 diffuse = light.diffuse * diff *  material.diffuse;

   //specular
   vec3 viewDir = normalize(viewPos - vsPos);
   vec3 reflectDIr = reflect(-lightDir, normal);
   float spec = 0.0;
   vec3 halfwayDir = normalize(lightDir + viewDir);
   spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
   vec3 specular = light.specular * (spec * material.specular); 


   fsColour = vec4(ambient + diffuse + specular, 1.0f);
}
