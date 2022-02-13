#version 410 core
layout(triangles, equal_spacing, ccw) in;

uniform mat4 gVP;
uniform float gDispFactor;
uniform bool gTessellate;
struct Material {
	sampler2D diffuse; //not using
	sampler2D specular; //not using
	sampler2D normal; //not using
	sampler2D height; 
	float shininess; //not using
};
uniform Material material;

uniform mat4 lightView;
uniform mat4 lightProjection;

in vec3 WorldPos_ES_in[];
in vec2 TexCoord_ES_in[];
in vec3 Normal_ES_in[];
in vec3 Tangent_ES_in[];
in vec3 BiTangent_ES_in[];

out vec3 WorldPos_GS_in;
out vec2 TexCoord_GS_in;
out vec3 Normal_GS_in;
out vec3 Tangent_GS_in;
out vec3 BiTangent_GS_in;
out vec4 FragPosLightSpace_GS_in;

vec2 interpolateVec2(vec2 v0, vec2 v1, vec2 v2)
{
	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolateVec3(vec3 v0, vec3 v1, vec3 v2)
{
	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
	// Interpolate the attributes of the output vertex using the barycentric coordinates
	TexCoord_GS_in = interpolateVec2(TexCoord_ES_in[0], TexCoord_ES_in[1], TexCoord_ES_in[2]);
	Normal_GS_in = interpolateVec3(Normal_ES_in[0], Normal_ES_in[1], Normal_ES_in[2]);
	Tangent_GS_in = interpolateVec3(Tangent_ES_in[0], Tangent_ES_in[1], Tangent_ES_in[2]);
	BiTangent_GS_in = interpolateVec3(BiTangent_ES_in[0], BiTangent_ES_in[1], BiTangent_ES_in[2]);
	WorldPos_GS_in = interpolateVec3(WorldPos_ES_in[0], WorldPos_ES_in[1], WorldPos_ES_in[2]);

	if (gTessellate)
	{
		// Displace the vertex along the normal
		float Displacement = texture(material.height, TexCoord_GS_in.xy).x;
		WorldPos_GS_in += Normal_GS_in * Displacement * gDispFactor;
	}

	FragPosLightSpace_GS_in = lightProjection * lightView * vec4(WorldPos_GS_in, 1);

	gl_Position = gVP * vec4(WorldPos_GS_in, 1.0);
}