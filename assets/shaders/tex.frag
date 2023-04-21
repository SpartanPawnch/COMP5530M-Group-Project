#version 330 core

in vec2 vsTex;
in vec3 vsNormal;

out vec4 fsColour;

void main()
{
  
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
}
