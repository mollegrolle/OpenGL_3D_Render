#version 410 core

// define the number of CPs in the output patch
layout(vertices = 3) out;

uniform vec3 gEyeWorldPos;
uniform bool gTessellate;

// attributes of the input CPs
in vec3 WorldPos_CS_in[];
in vec2 TexCoord_CS_in[];
in vec3 Normal_CS_in[];
in vec3 Tangent_CS_in[];
in vec3 BiTangent_CS_in[];

// attributes of the output CPs
out vec3 WorldPos_ES_in[];
out vec2 TexCoord_ES_in[];
out vec3 Normal_ES_in[];
out vec3 Tangent_ES_in[];
out vec3 BiTangent_ES_in[];

float GetTessLevel(float Distance0, float Distance1)
{
	float AvgDistance = (1000 / (Distance0 * Distance1));

	return AvgDistance;
}

void main()
{
	//default value 1.0f. No tessellation if not changed
	float tessLevel1 = 1.0f;
	float tessLevel2 = 1.0f;
	float tessLevel3 = 1.0f;

	// Set the control points of the output patch
	WorldPos_ES_in[gl_InvocationID] = WorldPos_CS_in[gl_InvocationID];
	TexCoord_ES_in[gl_InvocationID] = TexCoord_CS_in[gl_InvocationID];
	Normal_ES_in[gl_InvocationID] = Normal_CS_in[gl_InvocationID];
	Tangent_ES_in[gl_InvocationID] = Tangent_CS_in[gl_InvocationID];
	BiTangent_ES_in[gl_InvocationID] = BiTangent_CS_in[gl_InvocationID];

	//if true. change the value on tessLevel variable. Tessellate
	if (gTessellate)
	{
		// Calculate the distance from the camera to the three control points
		float EyeToVertexDistance0 = distance(gEyeWorldPos, WorldPos_ES_in[0]);
		float EyeToVertexDistance1 = distance(gEyeWorldPos, WorldPos_ES_in[1]);
		float EyeToVertexDistance2 = distance(gEyeWorldPos, WorldPos_ES_in[2]);

		tessLevel1 = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
		tessLevel2 = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
		tessLevel3 = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
	}
	// Calculate the tessellation levels
	gl_TessLevelOuter[0] = tessLevel1;
	gl_TessLevelOuter[1] = tessLevel2;
	gl_TessLevelOuter[2] = tessLevel3;
	gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}
