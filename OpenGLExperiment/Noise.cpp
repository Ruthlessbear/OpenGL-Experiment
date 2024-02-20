#include <iostream>
using namespace std;

#include <GL/glew.h>    
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"glm/gtx/rotate_vector.hpp"
#include <random>

float random(glm::vec2 st)
{
	return glm::fract(sin(glm::dot(glm::vec2(st.x, st.y), glm::vec2(12.9898, 78.233))) * 43758.5453123);
}

float noise2(glm::vec2 st) 
{
	glm::vec2 i = floor(st);
	glm::vec2 f = fract(st);

	float a = random(i);
	float b = random(i + glm::vec2(1.0, 0.0));
	float c = random(i + glm::vec2(0.0, 1.0));
	float d = random(i + glm::vec2(1.0, 1.0));

	glm::vec2 u = glm::smoothstep(0.0f, 1.0f, f);

	float result = glm::mix(a, b, u.x) +
		(c - a) * u.y * (1.0 - u.x) +
		(d - b) * u.x * u.y;
	return result;
}

GLubyte* CreateNoise2D()
{
	int noise2DTexSize = 512;
	GLubyte* noise2DTexPtr;

	int f, i, j, inc;
	int startFrequency = 4;
	int numOctaves = 4;
	double ni[2];
	double inci, incj;
	int frequency = startFrequency;
	GLubyte* ptr;
	double amp = 0.5f;

	if ((noise2DTexPtr = (GLubyte*)malloc(noise2DTexSize * noise2DTexSize * 4)) == NULL)
	{
		fprintf(stderr, "ERROR:create noise texture 2d failed");
		return NULL;
	}

	for (f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= 0.5f)
	{
		ptr = noise2DTexPtr;
		ni[0] = ni[1] = 0;
		
		for (i = 0; i < noise2DTexSize; ++i, ni[0] += inci)
		{
			inci = 1.0 / (noise2DTexSize / frequency);
			for (j = 0; j < noise2DTexSize; ++j, ni[1] += incj, ptr += 4)
			{
				incj = 1.0f / (noise2DTexSize / frequency);
				*(ptr + inc) = (GLubyte)((noise2(glm::vec2(ni[0], ni[1])) * amp) * 512);
			}
		}
	}

	return noise2DTexPtr;
}