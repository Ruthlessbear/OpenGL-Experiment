#version 450 core		
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;

out vec3 outColor;
out vec3 normalWS;
out vec3 positionWS;

uniform mat4 Transform_1;
uniform	mat4 ProjectionMatrix;
uniform	mat4 ViewMatrix;
uniform	mat4 NormalToWorldMt;

void main()
{
	gl_Position = ProjectionMatrix * ViewMatrix * Transform_1 * vec4(position, 1.0f);	// 核心函数(位置信息赋值)
	outColor = color;
	normalWS = normal;
	//normalWS = vec3(NormalToWorldMt * vec4(normal, 1.0f));
	positionWS = vec3(Transform_1 * vec4(position, 1.0f));
}