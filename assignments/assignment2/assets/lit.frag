#version 450
out vec4 FragColor; //The color of this fragment
in Surface{
	vec3 WorldPos; //Vertex position in world space
	vec3 WorldNormal; //Vertex normal in world space

	vec2 TexCoord;

	vec4 FragPosLightSpace;
}fs_in;

uniform sampler2D _MainTex;
uniform sampler2D _ShadowMap;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0); //straight down
uniform vec3 _LightColor = vec3(1.0); //White light
uniform vec3 _EyePos;
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);

struct Material{
	float Ka; //Ambient coefficient (0-1)
	float Kd; //Diffuse coefficient (0-1)
	float Ks; //Specular coefficient (0-1)
	float Shininess; //Affects size of specular highlight
};
uniform Material _Material;

float calcShadow(sampler2D shadowMap, vec4 lightSpacePos);
void main(){
	vec3 normal = normalize(fs_in.WorldNormal);

	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal,toLight),0.0);
	
	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);
	
	//Shadows
	float shadow = calcShadow(_ShadowMap, fs_in.FragPosLightSpace); 
	//vec3 lightColor = (_AmbientColor * _Material.Ka) + (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * (1.0 - shadow);

	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;
	lightColor *= (1.0 - shadow);
	lightColor += _AmbientColor * _Material.Ka;
	vec3 objectColor = texture(_MainTex,fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor,1.0);
}

float calcShadow(sampler2D shadowMap, vec4 lightSpacePos)
{
	vec3 sampleCoord = lightSpacePos.xyz / lightSpacePos.w;
	sampleCoord = sampleCoord * 0.5 + 0.5;
	float myDepth = sampleCoord.z; 
	float shadowMapDepth = texture(shadowMap, sampleCoord.xy).r;
	
	//step(a,b) returns 1.0 if a >= b, 0.0 otherwise
	return step(shadowMapDepth,myDepth);
}