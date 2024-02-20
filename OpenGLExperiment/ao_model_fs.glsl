#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D aoTexture;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0) ;
uniform float diffuse_factor = 1.2f;
uniform float specular_glass = 10.0f;
uniform mat4 view;
uniform float ssao_factor = 0.5f;
uniform vec4 ssao_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

const float linear = 0.09f;
const float quadratic = 0.032f;

void main()
{    
	vec3 v_fragPos = texture(gPositionDepth, TexCoords.xy).rgb;
	vec3 v_normal = texture(gNormal, TexCoords).rgb;
	float occlusion = texture(aoTexture, TexCoords.xy).r;

	vec3 lighting = vec3(0.0f, 0.0f, 0.0f);
	vec3 viewDir = normalize(-v_fragPos);//相机空间下相机位置(0, 0, 0)
	vec3 v_lightpos = (view * vec4(lightPos, 1.0f)).xyz;
	vec3 v_lightDir = normalize(v_lightpos - v_fragPos);

	//diffuse
	vec3 diffuse = max(dot(v_normal, v_lightDir), 0) * lightColor.xyz * diffuse_factor;
	
	//specular
	vec3 halfDir = normalize(v_lightDir + viewDir);
	float spec = pow(max(dot(v_normal, halfDir), 0.0f), specular_glass);
	vec3 specular = spec * lightColor.xyz;

	//attenuation
	float dist = length(v_lightpos - v_fragPos);
	float atten = 1.0f / (1.0f + linear * dist + quadratic * dist * dist);
	diffuse *= atten;
	specular *= atten;
	vec3 ambient = vec3(ssao_factor * occlusion.xxx) * ssao_color.rgb;
	lighting += diffuse + specular + ambient;
//	FragColor = vec4(lighting.xyz, 1.0f);
	FragColor = vec4(ambient, 1.0f);
}