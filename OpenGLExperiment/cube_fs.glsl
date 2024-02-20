#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 cube_color;

void main()
{    
    FragColor = vec4(cube_color.rgb, 1.0f);
}