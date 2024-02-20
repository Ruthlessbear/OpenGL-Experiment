#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
    vec3 tex_diffuse = texture(texture_diffuse1, TexCoords.xy).rgb;
    FragColor = vec4(tex_diffuse.rgb, 1.0f);
}