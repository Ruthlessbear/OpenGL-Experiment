#version 450 core
out vec4 FragColor;

uniform sampler2D show_texture;

in vec2 Texcoords;

void main()
{    
	vec3 tex = texture(show_texture, Texcoords.xy).rgb;
	FragColor = vec4(tex.xxx, 1.0f);
}