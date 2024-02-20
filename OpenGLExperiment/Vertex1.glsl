#version 450 core		// 3.30版本
layout(location = 0) in vec3 position;	// 三个浮点数vector向量表示位置 position变量名
layout(location = 1) in vec3 color;	

out V_OUT
{
	vec3 ourColor;
}v_out;

void main()
{
	gl_Position = vec4(position, 1.0f);	// 核心函数(位置信息赋值)
	v_out.ourColor = vec3(0.0, 1.0, 0.0);
}