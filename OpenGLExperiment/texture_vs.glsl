#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexcoords;

out vec2 Texcoords;

void main()
{
    Texcoords = aTexcoords;
    gl_Position = vec4(aPos, 1.0f);
}