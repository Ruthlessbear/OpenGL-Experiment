#version 450 core
out vec4 FragColor;

in vec2 Texcoord;

uniform sampler2D tex_shadow;

void main()
{    
    vec3 shadow_map = texture(tex_shadow, Texcoord.xy).rgb;
    FragColor = vec4(shadow_map.rgb, 1.0f);
}