#version 150
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	
} IN;

out vec4 FragColor;

void main(void){
	vec4 value = texture(diffuseTex, IN.texCoord).rgba;

if(value.a == 0.0){
discard; }

FragColor= value;
}