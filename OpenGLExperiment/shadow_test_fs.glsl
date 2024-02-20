#version 450 core
out vec4 FragColor;

in vec4 DepthPosTexCoords;

uniform sampler2DShadow texture_depth;

void main()
{    
    float shadow = textureProj(texture_depth, DepthPosTexCoords);
    FragColor = vec4(shadow.xxx, 1.0f);
}