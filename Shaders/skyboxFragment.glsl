# version 150 core

uniform samplerCube cubeTex;
uniform vec3;

in Vertex {
 vec3 normal ;
 } IN ;

out vec4 FragColour;

void main ( void ) {
FragColour = texture(cubeTex, normalize(IN.normal));
 }