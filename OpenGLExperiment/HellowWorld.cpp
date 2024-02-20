#include <iostream>		// C++�ı�׼�������ͷ�ļ�
using namespace std;

#define GLEW_STATIC     // Ԥ���壺�� glew �����ŵ�ϵͳ�ļ�������(STATIC:���ҵ���̬�������ʹ�þ�̬��)
// �ɾ�̬�⵼����Ӿ�̬�⵼��ĺ�����ǩ���ùؼ��� export .�Ӷ�̬�⵼��Ĺ��ܱ����� extern __declspec(dllimport).GLEW_STATIC�Ǽ����һ�������Ԥ����������.

#include <GL/glew.h>    // ������GL���ļ�������ġ�glew.h����ͷ�ļ�
						// �������е���һ�κ����ҵ���GL���ļ����򿪺��ҵ���glew.h��ͷ�ļ���Ȼ��
						// ���ڡ����롱��ʱ�����������δ��븴�Ƶ����棬ֻ��û����ʾ����

#include <GLFW/glfw3.h> // ������Ҫ�Ͳ�ͬ�Ĳ���ϵͳ���н���������Ҫ��glfw.h����ͷ�ļ�
						// �ȴ���һ�����ڣ�Ȼ���ڴ������洴��һ�����ӿڡ�

#include "ShaderModel.h"
#include "Camera.h"
//#include "object.h"
#include"SOIL2/stb_image.h"
#include <SOIL2/SOIL2.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"glm/gtx/rotate_vector.hpp"


const GLchar* vertexCode_1 = "#version 330 core\n"		// 3.30�汾
"layout(location = 0) in vec3 position_1;\n"			// ����������vector������ʾλ�� position������
"void main()\n"
"{\n"
"gl_Position = vec4(position_1, 1.0f);\n"				// ���ĺ���(λ����Ϣ��ֵ)
"}\n";

const GLchar* fragmentCode_1 = "#version 330 core\n"	// �汾��Ϣ
"out vec4 FragColor_1;\n"								//������ĸ����������ɵ�һ������ RGB+aerfa
"void main()\n"
"{\n"
"FragColor_1 = vec4(0.5f, 0.75f, 0.25f, 1.0f);\n"
"}\n";

const GLint WIDTH = 600, HEIGHT = 600;		// �����ô����Լ����С
											// ��openGL���棬������������ǰ�涼�и���д�ġ�GL��,���ԡ�GLint����ʵ�ں����ǡ�����int������˼
											// ��openGL����ĺ�������Сд�ġ�gl����ͷ

#pragma region DrawTriangle
GLfloat vertices_1[] =
{
	0.0f, 0.5f, 0.0f,		// �϶���
	-0.5f, -0.5f, 0.0f,		// �󶥵�
	0.5f, -0.5f, 0.0f,		// �Ҷ���

};

int DrawTriangle()
{
	/*
		˵����
			glfw �ṩ���ǻ���(�������������ԡ�glfw����GLFW����ͷ�Ķ���)
			glew ������ͼ(�������������ԡ�glew����GLFW����ͷ�Ķ���)
	*/

	glfwInit();   //��ʼ����ʹ��glfw����һ������

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);		// ���ô��ڰ汾����MAJOR���������汾��

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		// ���ô��ڰ汾����MAJOR�������汾��

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// GLFW_OPENGL_PROFILE�ø��ߴ��ڣ�����汾��Ϊ��openGL ��׼���ġ�
																	// openGL�õİ汾�á�CORE_PROFILE������ʾ��ָ����3.1�Ժ�İ汾 �°��

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// ƻ��ϵͳ��Ҫ���������(Windows�ɼӿɲ���)���������ã���ǰ��İ汾����

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);				//���Ŵ��ڵĹر�

	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "Draw Triangle", nullptr, nullptr);
	// �½�һ�����ڣ�"һ֧��ͬѧ���ڴ˳�û��"(����������)�����ô�������

	//  ��ʼΪ������������
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // ���ʵ��ռ�õ�֡�����С��֡�Ŀ���screenWidth��֡�ĸߴ���screenHeight��

	if (nullptr == window_1)	//�жϴ�������Ƿ�ɹ���ʧ�������һ����ʾ
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();		// glfw�ر�
		return -1;				// ���̽���
	}

	glfwMakeContextCurrent(window_1);   // ����ǰ���ڣ�׼���Ե�ǰ���ڽ��л�ͼ

	glewExperimental = GL_TRUE;			// �����������ʱ��˵�������ʱ���ٴ��������(��ǰ��������һ�����)

	if (glewInit() != GLEW_OK)			// �ж�glew��ʼ���Ƿ�ɹ���������ֵ�Ƿ�ɹ� ʧ�������һ����ʾ
	{
		cout << "Failed to initialise GLEW" << endl;
		glfwTerminate();				// �ر�glfw
		return -1;
	}

	GLuint vertexShader_1 = glCreateShader(GL_VERTEX_SHADER);		// ����������ɫ������
	glShaderSource(vertexShader_1, 1, &vertexCode_1, NULL);			// ��������ɫ�������ݴ�����
	glCompileShader(vertexShader_1);	// ���붥����ɫ��
	GLint flag;							// �����жϱ����Ƿ�ɹ�
	GLchar infoLog[512];				// 512���ַ�
	glGetShaderiv(vertexShader_1, GL_COMPILE_STATUS, &flag); // ��ȡ����״̬
	if (!flag)
	{
		glGetShaderInfoLog(vertexShader_1, 512, NULL, infoLog);   // �����  
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}

	GLuint fragmentShader_1 = glCreateShader(GL_FRAGMENT_SHADER);		// ����ƬԪ��ɫ������
	glShaderSource(fragmentShader_1, 1, &fragmentCode_1, NULL);			// ��������ɫ�������ݴ�����
	glCompileShader(fragmentShader_1);									// ���붥����ɫ��
	glGetShaderiv(fragmentShader_1, GL_COMPILE_STATUS, &flag);			// ��ȡ����״̬
	if (!flag)
	{
		glGetShaderInfoLog(fragmentShader_1, 512, NULL, infoLog);		// �����  
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}

	GLuint shaderProgram_1 = glCreateProgram();
	glAttachShader(shaderProgram_1, vertexShader_1);
	glAttachShader(shaderProgram_1, fragmentShader_1);
	glLinkProgram(shaderProgram_1);
	glGetProgramiv(shaderProgram_1, GL_LINK_STATUS, &flag);
	if (!flag)
	{
		glGetProgramInfoLog(shaderProgram_1, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}
	glDeleteShader(vertexShader_1);
	glDeleteShader(fragmentShader_1);

	GLuint VAO, VBO;				// �����ǳɶԳ��ֵ�
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// ���Կ��л���һ���ռ����
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_1), vertices_1, GL_STATIC_DRAW);	// GL_STATIC_DRAW����̬�Ļ�ͼ(Ƶ���ض�)

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window_1))		// ֻҪ��ǰ���ڲ��رգ�һֱִ�����ѭ��
	{
		glViewport(0, 0, screenWidth, screenHeight);	// �����ӿڵĴ�С(֡����Ĵ�С����ȥ)
													// ԭ���������� glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
													// (x ,y)�����ӿ�(������)�����½ǵ����ꡣwidth��height�ֱ�����ӿڵĿ�͸�
		glfwPollEvents();							// �¼���Ӧ��������ã��������е��¼���

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// �ֱ��Ǻ졢�̡�����͸���ȵ��ĸ�������RGB��ԭɫ+͸����(1.0��ʾ��͸����0.1��ʾ��ȫ͸��)
													// һ����Ե�RGB��ʾ�Ķ���8λ���ܱ�ʾ256*256*256=16777216ɫ
		glClear(GL_COLOR_BUFFER_BIT);				// glClear()��ʹ�� glClearColor ��ָ����ֵ�趨��ɫ��������ֵ�����������е�ÿһ����������Ϊ����ɫGL_COLOR_BUFFER_BIT

		glUseProgram(shaderProgram_1);		// ��Ⱦ������ɫ������
		glBindVertexArray(VAO);				// �� VAO
		glDrawArrays(GL_TRIANGLES, 0, 3);	// ��������  �ӵ�0�����㿪ʼ һ����3��
		glBindVertexArray(0);				// ���

		glfwSwapBuffers(window_1);					// ��˫����ģʽ(����֪ʶ),�൱���ó����顰���塱(һ�黭�õ�չʾ������ǰ����һ����Ż����������òŸ��㿴)
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram_1);

	glfwTerminate();   // ���ѭ��������glfw�ر�

	return 0;
}
#pragma endregion

#pragma region DrawRectangle
GLfloat vertices_rect[] =
{
	-0.5f, 0.5f, 0.0f,		// ����
	0.5f, 0.5f, 0.0f,		//����
	-0.5f, -0.5f, 0.0f,		// ����
	0.5f, -0.5f, 0.0f,		// ����

};
int DrawRectangle()
{
	glfwInit();								//��ʼ����ʹ��glfw����һ������
	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "DrawRectangle", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // ���ʵ��ռ�õ�֡�����С
	glfwMakeContextCurrent(window_1);   // ����ǰ���ڣ�׼���Ե�ǰ���ڽ��л�ͼ
	glewInit();

	//������ɫ��
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexCode_1, NULL);
	glCompileShader(vertexShader);
	GLint flag;
	GLchar infolog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &flag);
	if (!flag != NULL)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infolog << endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentCode_1, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &flag);
	if (!flag != NULL)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infolog << endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, NULL);
	if (!flag != NULL)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infolog);
		cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infolog << endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLuint VAO, VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_rect), vertices_rect, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window_1))
	{
		glViewport(0, 0, screenWidth, screenHeight);	// �����ӿڵĴ�С
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);		// �ֱ��Ǻ졢�̡�����͸���ȵ��ĸ�������RGB��ԭɫ+͸����
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		int length = sizeof(vertices_rect) / sizeof(vertices_rect[0]);
		int drawSize = length / 3;
		for (int i = 0; i <= drawSize - 3; i++)
		{
			glDrawArrays(GL_TRIANGLES, i, 3);
		}
		glBindVertexArray(0);

		glfwSwapBuffers(window_1);					// ˫����ģʽ
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();								// ���ѭ��������glfw�ر�
	return 0;
}
#pragma endregion

#pragma region DrawColorTriangle
GLfloat vertices_color[] =
{	// position				// color
	0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,		// �϶���(��ɫ)
	-0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,		// �󶥵�(��ɫ)
	0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f		// �Ҷ���(��ɫ)
};

//GLfloat vertices_color[] =
//{	// position				
//	0.0f, 0.5f, 0.0f,		
//	-0.5f, -0.5f, 0.0f,		
//	0.5f, -0.5f, 0.0f
//};

int DrawColorTriangle()
{
	glfwInit();								//��ʼ����ʹ��glfw����һ������
	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "DrawColorTriangle", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // ���ʵ��ռ�õ�֡�����С
	glfwMakeContextCurrent(window_1);   // ����ǰ���ڣ�׼���Ե�ǰ���ڽ��л�ͼ
	glewInit();

	ShaderModel shader = ShaderModel("Vertex1.glsl", "Fragment1.glsl", "TCS1.glsl", "TES1.glsl", "GeometryShader.txt");

	GLuint VAO, VBO;
	glCreateBuffers(1, &VBO);
	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_color), vertices_color, GL_STATIC_DRAW);
	//glCreateBuffers(1, &VBO);
	//glNamedBufferStorage(VBO, 1024 * 1024, NULL, GL_MAP_WRITE_BIT);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glNamedBufferSubData(VBO, 0, sizeof(vertices_color), vertices_color);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window_1))
	{
		glViewport(0, 0, screenWidth, screenHeight);	// �����ӿڵĴ�С
		glDepthRange(0.01, 1000);
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// �ֱ��Ǻ졢�̡�����͸���ȵ��ĸ�������RGB��ԭɫ+͸����
		glClear(GL_COLOR_BUFFER_BIT);

		shader.Use();
		glBindVertexArray(VAO);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPointSize(5.0);
		glDrawArrays(GL_PATCHES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window_1);					// ˫����ģʽ
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();								// ���ѭ��������glfw�ر�
	return 0;
}
#pragma endregion

#pragma region DrawChangeColorRect
GLfloat vertices_changeRect[] =
{
	//position					
	0.5f, 0.5f, 0.0f,			// top right		0
	0.5f, -0.5f, 0.0f,			// bottom right		1
	-0.5f, -0.5f, 0.0f,			// bottom left		2
	-0.5f, 0.5f, 0.0f,			// top left			3
};

GLuint indices_2[] =
{
	0, 1, 2,		// ���Ϊ 0��1��3 �Ķ�����ϳ�һ��������
	0, 2, 3			// ���Ϊ 1��2��3 �Ķ�����ϳ�һ��������
};

int DrawChangeColorRect()
{
	glfwInit();								//��ʼ����ʹ��glfw����һ������
	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "DrawChangeColorRect", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // ���ʵ��ռ�õ�֡�����С
	glfwMakeContextCurrent(window_1);   // ����ǰ���ڣ�׼���Ե�ǰ���ڽ��л�ͼ
	glewInit();

	ShaderModel shader = ShaderModel("Vertex2.txt", "Fragment2.txt");

	GLuint VAO, VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_changeRect), vertices_changeRect, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_2), &indices_2, GL_STATIC_DRAW);

	while (!glfwWindowShouldClose(window_1))
	{
		glViewport(0, 0, screenWidth, screenHeight);	// �����ӿڵĴ�С
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// �ֱ��Ǻ졢�̡�����͸���ȵ��ĸ�������RGB��ԭɫ+͸����
		glClear(GL_COLOR_BUFFER_BIT);

		shader.Use();
		float time = glfwGetTime();
		float redValue = sin(time) / 2.0f + 0.5f;
		float greenValue = 1 - redValue;
		int vertexColorLocation = glGetUniformLocation(shader.Program, "time");
		glUniform4f(vertexColorLocation, redValue, greenValue, 0.5f, 1.0f);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		int length = sizeof(indices_2) / sizeof(GLuint);
		glDrawElements(GL_TRIANGLES, length, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glfwSwapBuffers(window_1);					// ˫����ģʽ
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();								// ���ѭ��������glfw�ر�
	return 0;
}
#pragma endregion

#pragma region DrawTexture
GLfloat vertices_texture[] =
{
	//position				// texture coords(��������)
	0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		// ���϶���		���0
	0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		// ���¶���		���1
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		// ���¶���		���2
	-0.5f, 0.5f, 0.0f,		0.0f, 1.0f		// ���϶���		���3
};
int width, height;
unsigned char* image = SOIL_load_image("texture1.jpg", &width, &height, 0, SOIL_LOAD_RGBA);

GLuint indices_3[] =
{
	0, 1, 2,		// ���Ϊ 0��1��3 �Ķ�����ϳ�һ��������
	0, 2, 3			// ���Ϊ 1��2��3 �Ķ�����ϳ�һ��������
};

int DrawTexture()
{
	glfwInit();								//��ʼ����ʹ��glfw����һ������
	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "DrawTexture", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // ���ʵ��ռ�õ�֡�����С
	glfwMakeContextCurrent(window_1);   // ����ǰ���ڣ�׼���Ե�ǰ���ڽ��л�ͼ
	glewInit();

	ShaderModel shader = ShaderModel("Vertex3.txt", "Fragment3.txt");

	GLuint VAO, VBO;
	GLuint EBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_texture), vertices_texture, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_3), &indices_3, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Generate Texture
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	shader.Use();
	int textureLocation = glGetUniformLocation(shader.Program, "Texture");
	glUniform1i(textureLocation, 0);

	glm::mat4 unitMatrix = glm::mat4(1.0f);
	float scale = 1.0f;
	while (!glfwWindowShouldClose(window_1))
	{
		glViewport(0, 0, screenWidth, screenHeight);	// �����ӿڵĴ�С
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// �ֱ��Ǻ졢�̡�����͸���ȵ��ĸ�������RGB��ԭɫ+͸����
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		float time = glfwGetTime();
		float moveFactor = (sin(time) * 0.5f + 0.5f) * scale;
		glm::mat4 transformMove = glm::translate(unitMatrix, glm::vec3(0.0f, 1.0f - moveFactor, 0.0f));

		int uniformMatrix = glGetUniformLocation(shader.Program, "Transform_1");
		glUniformMatrix4fv(uniformMatrix, 1, GL_FALSE, glm::value_ptr(transformMove));

		shader.Use();
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		int length = sizeof(indices_3) / sizeof(GLuint);
		glDrawElements(GL_TRIANGLES, length, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window_1);					// ˫����ģʽ
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();								// ���ѭ��������glfw�ر�
	return 0;
}
#pragma endregion

#pragma region DrawCube
/* ��д������λ�� */
float vertices_cube[] = {
	// x��y��z ����				// color				// normal
	-0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,	// red ��ɫ��
	 0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,	// green ��ɫ��
	 0.5f, -0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 	// blue ��ɫ��(����ͼ��������)
	-0.5f,  0.5f, -0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,		1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,	// yellow ��ɫ��
	 0.5f,  0.5f, -0.5f,		1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,		1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,		1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,		1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,		1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f, 	// purple ��ɫ��
	 0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	-0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,	// cyan ����ɫ��
	 0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f
};


bool keys[1024];				// ר�Ŵ洢�����ļ�
Camera camera(glm::vec3(1.0f, 0.25f, -5.0f), glm::vec3(-1.0f, -1.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

void SquareMovement(GLfloat&, GLfloat&, GLfloat&);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
const double Shift_pix = 0.025f;
bool firstMouse = true;
GLfloat lastX = 0;
GLfloat lastY = 0;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		cout << "�����˹رռ� esc = " << key << endl;
	}
	if (key >= 0 && key <= 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;	// true �������˼�
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void Key_Movement()
{
	if (keys[GLFW_KEY_Q])		// ��ǰ
		camera.ProcessKeyboard(FORWARD, deltaTime);

	if (keys[GLFW_KEY_E])		// ���
		camera.ProcessKeyboard(BACKWARD, deltaTime);

	if (keys[GLFW_KEY_A])		// ����
		camera.ProcessKeyboard(LEFT, deltaTime);

	if (keys[GLFW_KEY_D])		// ����
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (keys[GLFW_KEY_W])		// ����
		camera.ProcessKeyboard(UPWARD, deltaTime);

	if (keys[GLFW_KEY_S])		// ����
		camera.ProcessKeyboard(DOWNWARD, deltaTime);
}

void SquareMovement(GLfloat& up_down_move, GLfloat& left_right_move, GLfloat& front_back_move)			// �������ƶ�
{
	if (keys[GLFW_KEY_UP])		// ����
	{
		up_down_move -= Shift_pix;
	}

	if (keys[GLFW_KEY_DOWN])	// ����
	{
		up_down_move += Shift_pix;
	}

	if (keys[GLFW_KEY_LEFT])	// ����
	{
		left_right_move -= Shift_pix;
	}

	if (keys[GLFW_KEY_RIGHT])	// ����
	{
		left_right_move += Shift_pix;
	}

	if (keys[GLFW_KEY_F])		// ��ǰ(�� F ��)
	{
		front_back_move -= Shift_pix;
	}

	if (keys[GLFW_KEY_B])		// ���(�� B ��)
	{
		front_back_move += Shift_pix;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
		return;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;			// ע���������෴��

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float yo = yoffset;
	camera.ProcessMouseScroll(yoffset);
}

unsigned char* uniform_buffer = (unsigned char*)malloc(512);

int DrawCube()
{
	glfwInit();								//��ʼ����ʹ��glfw����һ������
	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "DrawCube", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // ���ʵ��ռ�õ�֡�����С
	glfwMakeContextCurrent(window_1);   // ����ǰ���ڣ�׼���Ե�ǰ���ڽ��л�ͼ
	glfwSetInputMode(window_1, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// ��겶��(�Ҳ���ʾ)

	glfwSetKeyCallback(window_1, KeyCallback);						// ��������
	glfwSetCursorPosCallback(window_1, mouse_callback);				// �������
	glfwSetScrollCallback(window_1, scroll_callback);				// ��������
	glewInit();

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	ShaderModel shader = ShaderModel("Vertex4.glsl", "Fragment4.glsl");
	ShaderModel lightShader = ShaderModel("lightv.txt", "lightf.txt");

	GLuint VAO, VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cube), vertices_cube, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(2);

	static const GLchar* uniformnames[4] = {
		"TransformBlock.testColor"
	};

	GLuint uniformindices[1];
	glGetUniformIndices(shader.Program, 1, uniformnames, uniformindices);

	GLint uniformoffsets[1];
	GLint arraystrides[1];
	GLint matrixstrides[1];
	glGetActiveUniformsiv(shader.Program, 1, uniformindices, GL_UNIFORM_OFFSET, uniformoffsets);
	glGetActiveUniformsiv(shader.Program, 1, uniformindices, GL_UNIFORM_ARRAY_STRIDE, arraystrides);
	glGetActiveUniformsiv(shader.Program, 1, uniformindices, GL_UNIFORM_MATRIX_STRIDE, matrixstrides);

	//*((float*)(uniform_buffer + uniformoffsets[0])) = 0.5f;

	GLuint TransformBuffer;
	glGenBuffers(1, &TransformBuffer);
	glNamedBufferStorage(TransformBuffer, 64*64, uniform_buffer, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);

	GLuint TransformBlock_index = glGetUniformBlockIndex(shader.Program, "TransformBlock");
	glUniformBlockBinding(shader.Program, TransformBlock_index, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, TransformBuffer);

	GLfloat up_down_move = 0.0f;		// �����ƶ��ı���
	GLfloat left_right_move = 0.0f;		// �����ƶ��ı���
	GLfloat front_back_move = 0.0f;		// ǰ���ƶ��ı���

	while (!glfwWindowShouldClose(window_1))
	{
		glViewport(0, 0, screenWidth, screenHeight);	// �����ӿڵĴ�С
		glfwPollEvents();
		Key_Movement();			// ��ȡ���̵��Զ��嶯��
		SquareMovement(up_down_move, left_right_move, front_back_move);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// �ֱ��Ǻ졢�̡�����͸���ȵ��ĸ�������RGB��ԭɫ+͸����
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		lightShader.Use();
		int rotateSpeed = 1.0f;
		glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, -0.8f);
		glBindVertexArray(VAO);

		GLfloat time = glfwGetTime();
		deltaTime = time - lastTime;
		lastTime = time;

		glm::mat4 transMatrix = glm::mat4(1.0f);
		glm::mat4 normalMatrix = glm::mat4(1.0f);
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		glm::mat4 projectionMatrix = glm::perspective(camera.GetZoom(), (float)width / (float)height, 0.1f, 100.0f);

		transMatrix = glm::translate(transMatrix, glm::vec3(0.7f * cos(time), 0.7f * sin(time), 0.0f)); 
		transMatrix = glm::scale(transMatrix, glm::vec3(0.15f, 0.15f, 0.15f));
		transMatrix = glm::rotate(transMatrix, time, glm::vec3(0.0f, 1.0f, 0.0f));
		GLint transMatrixLocation_l = glGetUniformLocation(lightShader.Program, "Transform_1");
		glUniformMatrix4fv(transMatrixLocation_l, 1, GL_FALSE, glm::value_ptr(transMatrix));
		GLint viewMatrixLocation_l = glGetUniformLocation(lightShader.Program, "ViewMatrix");
		glUniformMatrix4fv(viewMatrixLocation_l, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		GLint projectionMatrixLocation_l = glGetUniformLocation(lightShader.Program, "ProjectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation_l, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glNamedBufferSubData(TransformBuffer, 0, 64*64, uniform_buffer);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glm::mat4 lightMatrix = glm::mat4(1.0f);
		lightMatrix = glm::rotate(lightMatrix, time, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPos = lightMatrix * glm::vec4(lightPos, 1.0f);

		shader.Use();
		transMatrix = glm::mat4(1.0f);
		transMatrix = glm::translate(transMatrix, glm::vec3(left_right_move, up_down_move, front_back_move));	// ת������
		transMatrix = glm::scale(transMatrix, glm::vec3(0.6f, 0.6f, 0.6f));
		//transMatrix = glm::rotate(transMatrix, time * rotateSpeed, glm::vec3(0.2f, 1.0f, 0.0f));
		//normalMatrix = glm::rotate(normalMatrix, time * rotateSpeed, glm::vec3(0.2f, 1.0f, 0.0f));
		//normalMatrix = glm::scale(normalMatrix, glm::vec3(0.6f, 0.6f, 0.6f));

		//Set vertex uniform param
		GLint transMatrixLocation = glGetUniformLocation(shader.Program, "Transform_1");
		glUniformMatrix4fv(transMatrixLocation, 1, GL_FALSE, glm::value_ptr(transMatrix));
		GLint viewMatrixLocation = glGetUniformLocation(shader.Program, "ViewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		GLint projectionMatrixLocation = glGetUniformLocation(shader.Program, "ProjectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		GLint NormalToWorldMatrixLocation = glGetUniformLocation(shader.Program, "NormalToWorldMt");
		glUniformMatrix4fv(NormalToWorldMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		//Set fragment uniform param
		GLint lightPosLocation = glGetUniformLocation(shader.Program, "lightPos");
		glUniform3f(lightPosLocation, lightPos.x, lightPos.y, lightPos.z);
		glm::vec3 cameraPos = camera.GetPosition();
		GLint viewPosLocation = glGetUniformLocation(shader.Program, "viewPos");
		glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		// ˫����ģʽ
		glfwSwapBuffers(window_1);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();								// ���ѭ��������glfw�ر�
	return 0;
}
#pragma endregion

#pragma region DrawDragon

int DrawDragon()
{
	glfwInit();								//��ʼ����ʹ��glfw����һ������
	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "DrawDragon", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // ���ʵ��ռ�õ�֡�����С
	glfwMakeContextCurrent(window_1);   // ����ǰ���ڣ�׼���Ե�ǰ���ڽ��л�ͼ
	glewInit();

	ShaderModel shader = ShaderModel("BlinnPhonevs.glsl", "BlinnPhonefr.glsl");

	//sb7::object dragonObject;
	//dragonObject.load("dragon.sbm");

	GLuint VAO, VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_color), vertices_color, GL_STATIC_DRAW);
	//glCreateBuffers(1, &VBO);
	//glNamedBufferStorage(VBO, 1024 * 1024, NULL, GL_MAP_WRITE_BIT);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glNamedBufferSubData(VBO, 0, sizeof(vertices_color), vertices_color);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window_1))
	{
		glViewport(0, 0, screenWidth, screenHeight);	// �����ӿڵĴ�С
		glDepthRange(0.01, 1000);
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// �ֱ��Ǻ졢�̡�����͸���ȵ��ĸ�������RGB��ԭɫ+͸����
		glClear(GL_COLOR_BUFFER_BIT);

		shader.Use();
		glBindVertexArray(VAO);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPointSize(5.0);
		glDrawArrays(GL_PATCHES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window_1);					// ˫����ģʽ
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();								// ���ѭ��������glfw�ر�
	return 0;
}
#pragma endregion

int main()
{
	DrawTriangle();
}