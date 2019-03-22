#version 150
uniform sampler2D diffuseTex;
uniform int useTexture;

in Vertex {
	vec2 texCoord;
vec4 color;
	
} IN;

out vec4 FragColor;

void main(void){
	FragColor = IN.color;
        if(useTexture > 0) {
             FragColor *= texture(diffuseTex, IN.texCoord); }
}