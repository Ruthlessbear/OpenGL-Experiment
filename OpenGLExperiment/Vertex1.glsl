#version 450 core		// 3.30�汾
layout(location = 0) in vec3 position;	// ����������vector������ʾλ�� position������
layout(location = 1) in vec3 color;	

out V_OUT
{
	vec3 ourColor;
}v_out;

void main()
{
	gl_Position = vec4(position, 1.0f);	// ���ĺ���(λ����Ϣ��ֵ)
	v_out.ourColor = vec3(0.0, 1.0, 0.0);
}