#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal; //multiplied with TBN
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;
uniform sampler2D gShadow;
uniform sampler2D gTangent;
uniform sampler2D gBiTangent;
uniform sampler2D gFaceNormal;

struct DirLight {
	vec3 direction;

	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

struct PointLight {
	vec3 position;

	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float radius;
};
const int NR_POINT_LIGHTS = 64;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {

	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 10.0f); //10.0f = shininess
	// combine results
	vec3 diffuse = light.diffuse * diff * vec3(texture(gDiffuse, TexCoords)) * texture(gShadow, TexCoords).r;
	vec3 specular = light.specular * spec * vec3(texture(gSpecular, TexCoords)) * texture(gShadow, TexCoords).r;

	return (diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	//SMALL LIGHTS
	float distance = length(light.position - fragPos);
	if (distance < light.radius)
	{
		vec3 lightDir = normalize(light.position - fragPos);
		// diffuse shading
		float diff = max(dot(normal, lightDir), 0.0);
		// specular shading
		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0f); //1.0f = shininess
		// attenuation
		float distance = length(light.position - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance +
			light.quadratic * (distance * distance));
		// combine results
		vec3 diffuse = light.diffuse * vec3(texture(gDiffuse, TexCoords)) * 2;
		vec3 specular = light.specular * vec3(texture(gSpecular, TexCoords));
		diffuse *= attenuation;
		specular *= attenuation;
		return (diffuse + specular);
	}
	else {
		return vec3(0.0f, 0.0f, 0.0f);
	}
}

void main()
{
	if (true) {
		vec3 FragPos = texture(gPosition, TexCoords).rgb;
		vec3 Normal = texture(gNormal, TexCoords).rgb;
		vec3 Diffuse = texture(gDiffuse, TexCoords).rgb;
		vec3 Tangent = texture(gTangent, TexCoords).rgb;
		vec3 BiTangent = texture(gBiTangent, TexCoords).rbg;
		vec3 FaceNormal = texture(gFaceNormal, TexCoords).rgb;

		// properties
		Normal = normalize(Normal);
		vec3 viewDir = normalize(viewPos - FragPos);

		// phase 1: Hard coded ambient
		vec3 ambient = vec3(0.0) * Diffuse; //low light ambient and increase contrast (diff * diff)
		vec3 result = vec3(0.0f) + ambient;
		// phase 2: Directional lighting
		result += CalcDirLight(dirLight, Normal, viewDir);
		// phase 3: Point lights
		for (int i = 0; i < NR_POINT_LIGHTS; i++)
			result += CalcPointLight(pointLights[i], Normal, FragPos, viewDir);  

		FragColor = vec4(result, 1.0);
	}
}