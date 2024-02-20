#version 450 core
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;

in vec3 v_normal;
in vec3 v_position;
in vec2 TexCoords;

const float near = 0.01f; //��ƽ��
const float far = 50.0f; //Զƽ��

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0f * near * far) / (far + near - z * (far - near));
}

void main()
{    
    gPositionDepth.rgb = v_position.xyz;
    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z);
    gNormal.rgb = normalize(v_normal).xyz;
}