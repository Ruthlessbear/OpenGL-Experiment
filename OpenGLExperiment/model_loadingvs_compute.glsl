#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out vec3 base_normal;
out vec3 world_position;
out mat3 TBN;

out vec2 TexCoords;
out vec4 DepthPosTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    //TBN
    vec3 b_tangent = normalize(cross(aTangent, aNormal));
    TBN = mat3x3(normalize(aTangent), b_tangent, normalize(aNormal));

    base_normal = mat3(transpose(inverse(model))) * aNormal;

    world_position = vec3(model * vec4(aPos, 1.0));
    TexCoords = vec2(aTexCoords.x, 1 - aTexCoords.y);    
    DepthPosTexCoords = lightSpaceMatrix * vec4(world_position.xyz, 1.0f);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}