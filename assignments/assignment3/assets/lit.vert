#version 450
//Vertex attributes
layout(location = 0) in vec3 vPos; //Vertex position in model space
layout(location = 1) in vec3 vNormal; //Vertex position in model space
layout(location = 2) in vec2 vTexCoord;

uniform mat4 _Model; //Model->World Matrix
uniform mat4 _ViewProjection; //Combined View->Projection Matrix
uniform mat4 lightSpaceMatrix;

//out vec3 Normal; //Output to next shader
out Surface{
	vec3 WorldPos; //Vertex position in world space
	vec3 WorldNormal;
	vec2 TexCoord;
	vec4 FragPosLightSpace;
}vs_out;


void main(){
	vs_out.WorldPos = vec3(_Model * vec4(vPos,1.0));
	vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * vNormal;
	vs_out.TexCoord = vTexCoord;
	vs_out.FragPosLightSpace = lightSpaceMatrix * _Model * vec4(vPos, 1);

	//Transform vertex position to homogeneous clip space
	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}
