#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;
//layout (location = 3) in vec3 aTangent;

out vec3 w_normal;
out vec3 w_position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    w_normal = mat3(transpose(inverse(model))) * aNormal;
    w_position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}