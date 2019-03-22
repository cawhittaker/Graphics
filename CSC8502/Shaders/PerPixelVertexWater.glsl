#version 150 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform float DeltaTime;
uniform sampler2D waterHeightTex;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec2 texCoord;

out Vertex {
vec4 colour;
vec2 texCoord;
vec3 normal;
vec3 worldPos;
} OUT;

void main(void) {
OUT . colour = vec4(1);
OUT . texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

OUT . normal = normalize(normalMatrix * normalize(normal));
OUT.worldPos = (modelMatrix * vec4(position,1)).xyz;

float tempval = DeltaTime/1000;
vec4 newTexCoord = vec4((texCoord*18.0)/257.0, 0, 1);

float height = texture(waterHeightTex, (textureMatrix*newTexCoord).xy).x;
vec3 newPos = vec3(
		position.x,
		position.y  + height*55.5, // height * 300
		position.z // sin(position.x + tempval)/3 + cos(position.z + tempval/2)/3
		);

gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(newPos , 1.0);
}