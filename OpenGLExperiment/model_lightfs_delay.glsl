#version 450 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0) ;

//diffuse factor
uniform float diffuse_factor = 1f;
//specular factor
uniform float specular_glass = 30f;
uniform float specular_factor = 1.0f;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;

void main()
{    
	vec3 w_pos = texture(gPosition, TexCoords.xy).rgb;
	vec3 w_normal = texture(gNormal, TexCoords.xy).rgb;
	vec3 w_diffuse = texture(gDiffuse, TexCoords.xy).rgb;
	vec3 w_specular = texture(gSpecular, TexCoords.xy).rgb;

	vec3 view_dir = normalize(cameraPos - w_pos);
	vec3 light_dir = normalize(lightPos - w_pos);
	vec3 half_dir = normalize(view_dir + light_dir);
	vec3 base_color = vec3(0.0, 0.0, 0.0);
	vec3 world_normal = w_normal;

    vec3 diffuse  = lightColor.xyz * w_diffuse.xyz * (max(dot(normalize(world_normal), light_dir), 0) * 0.5f + 0.5) * diffuse_factor;
    vec3 specular = lightColor.xyz * w_specular.xyz * pow(max(dot(normalize(world_normal), half_dir), 0), specular_glass) * specular_factor;
    FragColor = vec4((diffuse.xyz + specular.xyz), 1.0f);
}