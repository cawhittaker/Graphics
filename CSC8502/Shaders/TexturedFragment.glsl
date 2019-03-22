#version 150
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
	
} IN;

out vec4 FragColor;

void main(void){
	FragColor = texture(diffuseTex, IN.texCoord);
}