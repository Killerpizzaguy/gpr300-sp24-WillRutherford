#version 450
out vec4 FragColor; //The color of this fragment
in Surface{
	vec3 WorldPos; //Vertex position in world space
	vec3 WorldNormal; //Vertex normal in world space

	vec2 TexCoord;
}fs_in;

uniform sampler2D _MainTex;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0); //straight down
uniform vec3 _LightColor = vec3(1.0); //White light
uniform vec3 _EyePos;
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);

void main(){
	vec3 normal = normalize(fs_in.WorldNormal);

	vec3 toLight = -_LightDirection;
	float diffuseFactor = 0.5 * max(dot(normal,toLight),0.0); //Amount of light diffusely reflecting off surface
	
	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal,h),0.0),128);
	
	//Combination of specular and diffuse reflection
	vec3 lightColor = (diffuseFactor + specularFactor) * _LightColor;
	lightColor+=_AmbientColor;
	vec3 objectColor = texture(_MainTex,fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor,1.0);
}
