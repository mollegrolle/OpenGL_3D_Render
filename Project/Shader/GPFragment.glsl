#version 330

out vec4 FragColor;

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gDiffuse;
layout(location = 3) out vec3 gSpecular;
layout(location = 4) out float gShadow;
layout(location = 5) out vec3 gTangent;
layout(location = 6) out vec3 gBiTangent;
layout(location = 7) out vec3 gFaceNormal;

in vec3 WorldPos_FS_in;
in vec2 TexCoord_FS_in;
in vec3 Normal_FS_in;
in vec3 Tangent_FS_in;
in vec3 BiTangent_FS_in;
in mat3 TBN_FS_in; //not using
in vec4 FragPosLightSpace_FS_in;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D height; //not using
	float shininess; //not using
};
uniform Material material;

uniform sampler2D shadowMap;
uniform bool gTessellate;

uniform vec3 color;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide [clip space] range [-1,1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	float bias = 0.0025;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 0.0 : 1.0;
		}
	}
	shadow /= 9.0;
    return shadow;
}

void main()
{
	// shadow

	// store the fragment position vector in the first gbuffer texture vec3
	gPosition = WorldPos_FS_in;

	// also store the per-fragment normals into the gbuffer vec3
	gNormal = normalize(texture(material.normal, TexCoord_FS_in).rgb);
	gNormal = normalize(gNormal * 2.0 - 1.0);
	gNormal = normalize(TBN_FS_in * gNormal);

	// and the diffuse per-fragment color vec3
	gDiffuse = texture(material.diffuse, TexCoord_FS_in).rgb;

	if (gTessellate)
	{
		// coloring the height map on the tessellated object
		gDiffuse = vec3(WorldPos_FS_in.y, WorldPos_FS_in.y, WorldPos_FS_in.y);
	}

	// store the spec
	gSpecular = texture(material.specular, TexCoord_FS_in).rgb;

	gShadow = ShadowCalculation(FragPosLightSpace_FS_in);

	gTangent = Tangent_FS_in;

	gBiTangent = BiTangent_FS_in;

	gFaceNormal = Normal_FS_in;
}