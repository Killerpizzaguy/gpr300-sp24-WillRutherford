#version 450
//Code coppied from https://learnopengl.com/Advanced-OpenGL/Framebuffers
out vec4 FragColor;

in vec2 UV;

uniform sampler2D _ColorBuffer;
void main(){
	//vec3 color = 1.0-texture(_ColorBuffer,UV).rgb;
	//FragColor = vec4(color,1.0);
	FragColor = texture(_ColorBuffer, UV);
}
