#version 450
out vec4 FragColor; //The color of this fragment
in vec2 UV;

uniform layout(binding = 0) sampler2D _gPositions;
uniform layout(binding = 1) sampler2D _gNormals;
uniform layout(binding = 2) sampler2D _gAlbedo;
//uniform layout(binding = 3) sampler2D _ShadowMap;


uniform sampler2D _MainTex;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0); //straight down
uniform vec3 _LightColor = vec3(1.0); //White light
uniform vec3 _EyePos;
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);
uniform float _MinShadowBias;
uniform float _MaxShadowBias;

struct PointLight{
	vec3 position;
	float radius;
	vec3 color;
};
#define MAX_POINT_LIGHTS 256

//Anything passed into a std140 layout must have vectors that line up in chunks of 16 bytes
//A float has 4 bytes and a vec4 has 16
layout (std140, binding=0) uniform AdditionalLights{ 
	PointLight _PointLights[MAX_POINT_LIGHTS];
};



struct Material{
	float Ka; //Ambient coefficient (0-1)
	float Kd; //Diffuse coefficient (0-1)
	float Ks; //Specular coefficient (0-1)
	float Shininess; //Affects size of specular highlight
};
uniform Material _Material;
//
//struct Light {
//    vec3 Position;
//    vec3 Color;
//};

float calcShadow(sampler2D shadowMap, vec4 lightSpacePos, float bias);
vec3 calcPointLight(PointLight light,vec3 normal,vec3 pos);
void main(){
	vec3 normal = texture(_gNormals,UV).xyz;
	vec3 worldPos = texture(_gPositions,UV).xyz;
	vec3 albedo = texture(_gAlbedo,UV).xyz;

	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal,toLight),0.0);
	
	vec3 toEye = normalize(_EyePos - worldPos);
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);
	
	//Shadows
	float bias = max(_MaxShadowBias * (1.0 - dot(normal,toLight)),_MinShadowBias);
	//float shadow = calcShadow(_ShadowMap, fs_in.FragPosLightSpace, bias); 

	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;
	for(int i=0;i<MAX_POINT_LIGHTS;i++)
	{
	lightColor += calcPointLight(_PointLights[i], normal, worldPos);
	}

	//lightColor *= (1.0 - shadow);
	lightColor += _AmbientColor * _Material.Ka;
	FragColor = vec4(albedo * lightColor,1.0);
}

float calcShadow(sampler2D shadowMap, vec4 lightSpacePos, float bias)
{
	vec3 sampleCoord = lightSpacePos.xyz / lightSpacePos.w;
	sampleCoord = sampleCoord * 0.5 + 0.5;
	float myDepth = sampleCoord.z - bias; 
	float shadowMapDepth = texture(shadowMap, sampleCoord.xy).r;
	
	//step(a,b) returns 1.0 if a >= b, 0.0 otherwise
	return step(shadowMapDepth,myDepth);
}

//Linear falloff
float attenuateLinear(float distance, float radius){
	return clamp((radius-distance)/radius,0.0,1.0);
}
//Exponential falloff
float attenuateExponential(float distance, float radius){
	float i = clamp(1.0 - pow(distance/radius,4.0),0.0,1.0);
	return i * i;
	
}

vec3 calcPointLight(PointLight light,vec3 normal,vec3 pos){
	vec3 diff = light.position - pos;
	//Direction toward light position
	vec3 toLight = normalize(diff);

	float diffuseFactor = max(dot(normal,toLight),0.0);

	vec3 toEye = normalize(_EyePos - pos);
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);

	vec3 lightColor = (diffuseFactor + specularFactor) * light.color;
	
	//Attenuation
	float d = length(diff); //Distance to light
	lightColor*=attenuateExponential(d,light.radius);
	return lightColor;
}
