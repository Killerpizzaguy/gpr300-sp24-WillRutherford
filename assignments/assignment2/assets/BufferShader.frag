#version 450
//Code coppied from https://learnopengl.com/Advanced-OpenGL/Framebuffers
out vec4 FragColor;

in vec2 UV;

uniform sampler2D _ColorBuffer;
void main(){
	FragColor = texture(_ColorBuffer, UV);
}
