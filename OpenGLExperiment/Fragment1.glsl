#version 450 core	// �汾��Ϣ

in V_OUT
{
	vec3 ourColor;
}f_in;

out vec4 color;

void main()
{
	color = vec4(0.0, 1.0, 0.0, 1.0f);
}