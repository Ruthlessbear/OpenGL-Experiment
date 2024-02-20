#version 450 core
layout (location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexcoord;

out vec2 Texcoord;

void main()
{
    Texcoord = vec2(aTexcoord.x, aTexcoord.y);
    gl_Position = vec4(aPos, 1.0);
}