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

uniform vec3 viewPos;

void main()
{
   vec3 normalCol;
   if(vsNormal.x < 0.0)
   {
        normalCol.x = -vsNormal.x; 
   }
   else{ normalCol.x = vsNormal.x;
   }

   if(vsNormal.y < 0.0)
   {
        normalCol.y = -vsNormal.y; 
   }
   else{ normalCol.y = vsNormal.y;
   }

   if(vsNormal.z < 0.0)
   {
        normalCol.z = -vsNormal.z; 
   }
   else{ normalCol.z = vsNormal.z;
   }
   vec3 color = normalCol;


   //using normal instead of color
   //ambient
   vec3 ambient = 0.05 * color;
   //diffuse
   vec3 lightDir = normalize(lightPos - vsPos);
   vec3 normal = normalize(vsNormal);
   float diff = max(dot(lightDir, normal), 0.0);
   vec3 diffuse = diff * color;

   //specular
   vec3 viewDir = normalize(viewPos - vsPos);
   vec3 reflectDIr = reflect(-lightDir, normal);
   float spec = 0.0;
   vec3 reflectDir = reflect(-lightDir, normal);
   vec3 halfwayDir = normalize(lightDir + viewDir);
   spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
   vec3 specular = vec3(0.7) * spec; // assuming bright white light color


   fsColour = vec4(ambient + diffuse + specular, 1.0f);
}
