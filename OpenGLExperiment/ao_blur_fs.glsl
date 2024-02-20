#version 450 core
layout (location = 0) out float FragColor;

in vec2 Texcoords;

uniform sampler2D ssaoTexture;
const int blurSize = 4;//Ä£ºýºË

void main()
{    
	vec2 texelSize = 1.0f / vec2(textureSize(ssaoTexture, 0));
	float result = 0.0f;
	int blur_edge = int(floor(blurSize / 2.0f));
	for(int i = -blur_edge; i < blur_edge; i++)
	{
		for(int j = -blur_edge; j < blur_edge; j++)
		{
			vec2 offset = (vec2(0.0f) + vec2(float(i), float(j))) * texelSize;
			result += texture(ssaoTexture, Texcoords + offset).r;
		}
	}
	FragColor = result / (blurSize * blurSize);
}