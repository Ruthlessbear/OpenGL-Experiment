#version 450 core

in vec3 outColor;
in vec3 normalWS;
in vec3 positionWS;

out vec4 FragColor_1;

uniform TransformBlock
{
	float testColor;
} transform;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	float ambientStrength = 0.05f;
	vec3 ambient = vec3(1.0f, 1.0f, 1.0f) * ambientStrength;

	//Diffuse
	vec3 normalW = normalize(normalWS);
	vec3 lightDir = normalize(lightPos - positionWS);
	float diffuseStrength = 0.6f;
	float diffuse = (max(dot(normalW, lightDir), 0.0f) * 0.5f + 0.5f) * diffuseStrength;

	//Specular
	vec3 viewDir = normalize(viewPos - positionWS);
	vec3 h = normalize(lightDir + viewDir);

	float specularStrength = 1.3f;
	float specular = pow(max(dot(h, normalW), 0.0f), 64) * specularStrength;

	vec3 resultColor = (specular + diffuse) * outColor + ambient;

	FragColor_1 = vec4(resultColor, 1.0f);
}