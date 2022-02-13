#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
//change to line_strip to see structure

in vec3 WorldPos_GS_in[];
in vec2 TexCoord_GS_in[];
in vec3 Normal_GS_in[];
in vec3 Tangent_GS_in[];
in vec3 BiTangent_GS_in[];
in vec4 FragPosLightSpace_GS_in[];

out vec3 WorldPos_FS_in;
out vec2 TexCoord_FS_in;
out vec3 Normal_FS_in;
out vec3 Tangent_FS_in;
out vec3 BiTangent_FS_in;
out mat3 TBN_FS_in; //not using
out vec4 FragPosLightSpace_FS_in;

uniform vec3 viewPos;
uniform mat4 model;

float backFaceCull() {
	// Calculate two vectors in the plane of the input triangle
	vec3 ab = WorldPos_GS_in[1] - WorldPos_GS_in[0];
	vec3 ac = WorldPos_GS_in[2] - WorldPos_GS_in[0];
	vec3 face_normal = normalize((cross(ab, ac)));

	//viewdDir
	vec3 vd = normalize(WorldPos_GS_in[0] - viewPos);

	// Take the dot product of the normal with the view direction
	float d = dot(face_normal, vd);

	return d;
}

vec3 calcNormal() {

	// Calculate two vectors in the plane of the input triangle
	vec3 ab = WorldPos_GS_in[1] - WorldPos_GS_in[0];
	vec3 ac = WorldPos_GS_in[2] - WorldPos_GS_in[0];
	return normalize((cross(ab, ac)));

}

void main() {


	if (backFaceCull() <= 0.0)
	{

		for (int i = 0; i < 3; i++)
		{
			WorldPos_FS_in = WorldPos_GS_in[i];
			TexCoord_FS_in = TexCoord_GS_in[i];
			Normal_FS_in = Normal_GS_in[i];
			Tangent_FS_in = Tangent_GS_in[i];
			BiTangent_FS_in = BiTangent_GS_in[i];

			//Tangent
			vec3 T = normalize(vec3(model * vec4(Tangent_GS_in[i], 0.0)));
			vec3 N = normalize(vec3(model * vec4(Normal_GS_in[i], 0.0))); //Try calcNormal() here
			vec3 B = normalize(vec3(model * vec4(BiTangent_GS_in[i], 0.0)));
			// re-orthogonalize T with respect to N
			//T = normalize(T - dot(T, N) * N);
			// then retrieve perpendicular vector B with the cross product of T and N
			//B = cross(N, T);
			TBN_FS_in = mat3(T, B, N);

			FragPosLightSpace_FS_in = FragPosLightSpace_GS_in[i];

			gl_Position = gl_in[i].gl_Position;
			EmitVertex();
		}
		EndPrimitive();

	}
}