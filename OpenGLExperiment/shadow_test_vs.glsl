#version 450 core
layout (location = 0) in vec3 aPos;

out vec4 DepthPosTexCoords;

uniform mat4 lightSpaceMatrix;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
    DepthPosTexCoords = lightSpaceMatrix * model * vec4(aPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}