#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

out vec3 WorldPos_CS_in;
out vec2 TexCoord_CS_in;
out vec3 Normal_CS_in;
out vec3 Tangent_CS_in;
out vec3 BiTangent_CS_in;

uniform mat4 model; //worldMatrix

void main() {
	WorldPos_CS_in = vec4(model * vec4(vertexPosition, 1.0)).xyz;
	TexCoord_CS_in = vec2(vertexUV.x, vertexUV.y);
	Normal_CS_in = vertexNormal;
	Tangent_CS_in = vertexTangent;
	BiTangent_CS_in = vertexBitangent;
}