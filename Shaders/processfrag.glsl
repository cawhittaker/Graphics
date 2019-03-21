#version 150

uniform sampler2D diffuseTex;
uniform vec2 pixelSize;
uniform int isVertical;

in Vertex{
in vec2 texCoord;
in vec4 colour;
} IN;


out vec4 FragColour;
const float weights[5] = float[] (0.12, 0.22, 0.32, 0.22 ,0.12);

void main(void) {
vec2 values[5];

if(isVertical ==1) {
values = vec2[](vec2(0.0, -pixelSize.y*2), vec2(0.0,-pixelSize.y*1), vec2(0.0, 0.0), vec2(0.0, pixelSize.y*1), vec2(0.0, pixelSize.y*2)); }

else {
values = vec2[](vec2(0.0, -pixelSize.x*2), vec2(0.0,-pixelSize.x*1), vec2(0.0, 0.0), vec2(0.0, pixelSize.x*1), vec2(0.0, pixelSize.x*2)); }

for(int i =0; i<5; i++) {
vec4 tmp = texture2D(diffuseTex, IN.texCoord.xy + values[i]) * vec4(0.95,0.98,1.0,1.0);
FragColour += tmp * weights[i] ;}

}
