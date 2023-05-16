#version 330 core

in vec3 vsPos;
in vec3 vsNormal;
in vec2 vsTex;
in vec3 vsTangent;
in vec3 vsBitangent;

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

Material material;

const int MAX_LIGHTS = 20;
uniform int numLights;
uniform Light lights[MAX_LIGHTS];

uniform float gamma;
uniform vec3 viewPos;

uniform sampler2D baseColorSampler;
uniform sampler2D roughnessSampler;
uniform sampler2D metalnessSampler;
uniform sampler2D normalSampler;
uniform sampler2D alphaSampler;
uniform sampler2D emissiveSampler;
uniform sampler2D occlusionSampler;

uniform samplerCube skybox;

vec3 computeBlinnPhongLighting(int lightIndex)
{
    vec3 ambient = lights[lightIndex].ambient * material.ambient;

    //diffuse
    vec3 lightDir = normalize(lights[lightIndex].position - vsPos);
    vec3 normal = normalize(vsNormal);
    float diff = clamp(dot(lightDir, vsNormal), 0, 1);
    vec3 diffuse = lights[lightIndex].diffuse * diff *  material.diffuse;

    //specular
    vec3 viewDir = normalize(viewPos - vsPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * lights[lightIndex].specular * material.specular; 

    return (ambient+diffuse+specular);
}

vec3 F0(){
	return (1.0-texture( metalnessSampler, vsTex ).r)*vec3(0.04,0.04,0.04)+texture( metalnessSampler, vsTex ).r*texture( baseColorSampler, vsTex ).rgb;
}

vec3 F(vec3 h,vec3 v){
	return F0()+(1.0-F0())*pow((1.0-dot(h,v)),5.0);
}

vec3 Ldiffuse(vec3 h,vec3 v){
	float pi = 3.14159265359;
	return vec3(texture( baseColorSampler, vsTex ).rgb/pi*(vec3(1.0,1.0,1.0)-F(h,v))*(1.0-texture( metalnessSampler, vsTex ).r));
}

float ap(){
	return 2.0/(pow(texture( roughnessSampler, vsTex ).r,4.0)+0.001)-2.0;
}

float D(vec3 n,vec3 h){
	float pi = 3.14159265359;
	return (ap()+2.0)/(2.0*pi)*pow(max(0.0,dot(n,h)),ap());
}

float G(vec3 n, vec3 l,vec3 v,vec3 h){
	return min(1.0,min(2.0*((max(0.0,dot(n,h))*max(0.0,dot(n,v)))/dot(v,h)),2.0*((max(0.0,dot(n,h))*max(0.0,dot(n,l)))/dot(v,h))));
}

vec3 fr(vec3 h,vec3 l,vec3 v,vec3 n){
	//fr = Ldiffuse + (D(n,h)F(l,h)G(n,l,v))/(4(n*v)clamped (n*l)clamped)
	return  vec3(Ldiffuse(h,v) + (D(n,h)*F(h,v)*G(n,l,v,h))/(4.0*max(0.0,dot(n,v))* max(0.0,dot(n,l))+0.0001));
}

vec3 computePBRLighting(int lightIndex, vec3 n, vec3 v){
    vec3 l = normalize(lights[lightIndex].position-vsPos);
	vec3 h = normalize(l+v);
    vec3 reflection = reflect(-v, n);
    vec3 reflectionColor = textureCube(skybox, reflection).rgb;

    vec3 clight = lights[lightIndex].diffuse;
    vec3 frValue = fr(h,l,v,n);
    frValue += reflectionColor * max(0.0, dot(n, reflection));
    vec3 Lambient =lights[lightIndex].ambient * texture(baseColorSampler,vsTex).rgb * texture(occlusionSampler,vsTex).rgb;
    return Lambient + frValue * clight * max(0,dot(n,l));
}

vec3 computeNormalColor()
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
    return normal;
}


void main()
{
    
//    material.ambient = vec3(0.2, 0.2, 0.2);
//    material.diffuse = vec3(0.8, 0.8, 0.8);
//    material.specular = vec3(0.2, 0.2, 0.2);
//    material.shininess = 16.0;
//   
//    vec3 colour = texture(baseColorSampler,vsTex).rgb;
//    
//    vec3 lighting = vec3(0.0);
//    for(int i = 0; i < numLights; i++)
//    {
//        lighting += computeBlinnPhongLighting(i);
//    }
//    if(numLights>0){
//        fsColour.rgb = colour * lighting;
//        fsColour.rgb = pow(fsColour.rgb, vec3(1.0/gamma));
//        fsColour.a=1.f;
//    } else {
//        fsColour = vec4(colour,1);
//    }

    if(texture( alphaSampler, vsTex ).a<0.5){
		discard;
	}

    vec3 v = normalize(viewPos.rgb-vsPos);

    mat3 TBN = mat3(vsTangent,vsBitangent,vsNormal);

    vec3 n = texture(normalSampler, vsTex).rgb;
	n = n * 2.0 - 1.0;   
	n = normalize(TBN * n);

    vec3 lighting = vec3(0);
    for(int i = 0; i<numLights; i++){
        lighting += computePBRLighting(i,n,v);
    }
    fsColour = vec4(pow(lighting, vec3(1.0/gamma)),1);
}
