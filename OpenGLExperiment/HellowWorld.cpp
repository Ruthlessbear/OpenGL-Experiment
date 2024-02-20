#include <iostream>		// C++的标准输入输出头文件
using namespace std;

#define GLEW_STATIC     // 预定义：把 glew 这个库放到系统文件夹里面(STATIC:能找到静态库就优先使用静态库)
// 由静态库导出或从静态库导入的函数的签名用关键字 export .从动态库导入的功能必须用 extern __declspec(dllimport).GLEW_STATIC是激活第一种情况的预处理器定义.

#include <GL/glew.h>    // 包含“GL”文件夹里面的“glew.h”的头文件
						// 程序运行到这一段后，先找到“GL”文件，打开后找到“glew.h”头文件，然后
						// 会在“编译”的时候把里面的整段代码复制到下面，只是没有显示出来

#include <GLFW/glfw3.h> // 我们需要和不同的操作系统进行交互，则需要“glfw.h”的头文件
						// 先创建一个窗口，然后在窗口里面创建一个“视口”

#include "ShaderModel.h"
#include "Camera.h"
//#include "object.h"
#include"SOIL2/stb_image.h"
#include <SOIL2/SOIL2.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"glm/gtx/rotate_vector.hpp"


const GLchar* vertexCode_1 = "#version 330 core\n"		// 3.30版本
"layout(location = 0) in vec3 position_1;\n"			// 三个浮点数vector向量表示位置 position变量名
"void main()\n"
"{\n"
"gl_Position = vec4(position_1, 1.0f);\n"				// 核心函数(位置信息赋值)
"}\n";

const GLchar* fragmentCode_1 = "#version 330 core\n"	// 版本信息
"out vec4 FragColor_1;\n"								//输出是四个浮点数构成的一个向量 RGB+aerfa
"void main()\n"
"{\n"
"FragColor_1 = vec4(0.5f, 0.75f, 0.25f, 1.0f);\n"
"}\n";

const GLint WIDTH = 600, HEIGHT = 600;		// 先设置窗口以及其大小
											// 在openGL里面，数据类型名字前面都有个大写的“GL”,所以“GLint”其实内涵就是“整型int”的意思
											// 而openGL里面的函数都以小写的“gl”开头

#pragma region DrawTriangle
GLfloat vertices_1[] =
{
	0.0f, 0.5f, 0.0f,		// 上顶点
	-0.5f, -0.5f, 0.0f,		// 左顶点
	0.5f, -0.5f, 0.0f,		// 右顶点

};

int DrawTriangle()
{
	/*
		说明：
			glfw 提供的是环境(变量名或函数名以“glfw”或“GLFW”开头的都是)
			glew 用来绘图(变量名或函数名以“glew”或“GLFW”开头的都是)
	*/

	glfwInit();   //初始化，使用glfw来打开一个窗口

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);		// 设置窗口版本，“MAJOR”代表主版本号

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		// 设置窗口版本，“MAJOR”代表副版本号

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// GLFW_OPENGL_PROFILE用告诉窗口，这个版本是为了openGL 做准备的。
																	// openGL用的版本用“CORE_PROFILE”来表示，指的是3.1以后的版本 新版的

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// 苹果系统需要加这条语句(Windows可加可不加)。函数作用：向前面的版本兼容

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);				//缩放窗口的关闭

	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "Draw Triangle", nullptr, nullptr);
	// 新建一个窗口，"一支王同学再在此出没！"(第三个参数)：设置窗口名字

	//  开始为高清屏做设置
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // 获得实际占用的帧缓存大小。帧的宽传给screenWidth；帧的高传给screenHeight。

	if (nullptr == window_1)	//判断窗口输出是否成功，失败则给出一段提示
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();		// glfw关闭
		return -1;				// 进程结束
	}

	glfwMakeContextCurrent(window_1);   // 捕获当前窗口，准备对当前窗口进行画图

	glewExperimental = GL_TRUE;			// 如果程序编译的时候说有问题的时候，再打开这条语句(以前解决问题的一条语句)

	if (glewInit() != GLEW_OK)			// 判断glew初始化是否成功，看返回值是否成功 失败则给出一段提示
	{
		cout << "Failed to initialise GLEW" << endl;
		glfwTerminate();				// 关闭glfw
		return -1;
	}

	GLuint vertexShader_1 = glCreateShader(GL_VERTEX_SHADER);		// 创建顶点着色器对象
	glShaderSource(vertexShader_1, 1, &vertexCode_1, NULL);			// 将顶点着色器的内容传进来
	glCompileShader(vertexShader_1);	// 编译顶点着色器
	GLint flag;							// 用于判断编译是否成功
	GLchar infoLog[512];				// 512个字符
	glGetShaderiv(vertexShader_1, GL_COMPILE_STATUS, &flag); // 获取编译状态
	if (!flag)
	{
		glGetShaderInfoLog(vertexShader_1, 512, NULL, infoLog);   // 缓冲池  
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}

	GLuint fragmentShader_1 = glCreateShader(GL_FRAGMENT_SHADER);		// 创建片元着色器对象
	glShaderSource(fragmentShader_1, 1, &fragmentCode_1, NULL);			// 将顶点着色器的内容传进来
	glCompileShader(fragmentShader_1);									// 编译顶点着色器
	glGetShaderiv(fragmentShader_1, GL_COMPILE_STATUS, &flag);			// 获取编译状态
	if (!flag)
	{
		glGetShaderInfoLog(fragmentShader_1, 512, NULL, infoLog);		// 缓冲池  
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

	GLuint VAO, VBO;				// 它俩是成对出现的
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// 从显卡中划分一个空间出来
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_1), vertices_1, GL_STATIC_DRAW);	// GL_STATIC_DRAW：静态的画图(频繁地读)

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window_1))		// 只要当前窗口不关闭，一直执行这个循环
	{
		glViewport(0, 0, screenWidth, screenHeight);	// 设置视口的大小(帧缓存的大小传进去)
													// 原函数声明： glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
													// (x ,y)代表视口(正方形)的左下角的坐标。width、height分别代表视口的宽和高
		glfwPollEvents();							// 事件相应的命令，作用：捕获所有的事件。

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// 分别是红、绿、蓝、透明度的四个参数。RGB三原色+透明度(1.0表示不透明，0.1表示完全透明)
													// 一般电脑的RGB显示的都是8位，能表示256*256*256=16777216色
		glClear(GL_COLOR_BUFFER_BIT);				// glClear()：使用 glClearColor 中指定的值设定颜色缓存区的值，即将窗口中的每一个像素设置为背景色GL_COLOR_BUFFER_BIT

		glUseProgram(shaderProgram_1);		// 渲染调用着色器程序
		glBindVertexArray(VAO);				// 绑定 VAO
		glDrawArrays(GL_TRIANGLES, 0, 3);	// 画三角形  从第0个顶点开始 一共画3次
		glBindVertexArray(0);				// 解绑定

		glfwSwapBuffers(window_1);					// 打开双缓存模式(进阶知识),相当于拿出两块“画板”(一块画好的展示在你面前，另一块接着画，反正画好才给你看)
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram_1);

	glfwTerminate();   // 如果循环结束：glfw关闭

	return 0;
}
#pragma endregion

#pragma region DrawRectangle
GLfloat vertices_rect[] =
{
	-0.5f, 0.5f, 0.0f,		// 左上
	0.5f, 0.5f, 0.0f,		//右上
	-0.5f, -0.5f, 0.0f,		// 左下
	0.5f, -0.5f, 0.0f,		// 右下

};
int DrawRectangle()
{
	glfwInit();								//初始化，使用glfw来打开一个窗口
	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "DrawRectangle", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // 获得实际占用的帧缓存大小
	glfwMakeContextCurrent(window_1);   // 捕获当前窗口，准备对当前窗口进行画图
	glewInit();

	//顶点着色器
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
		glViewport(0, 0, screenWidth, screenHeight);	// 设置视口的大小
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);		// 分别是红、绿、蓝、透明度的四个参数。RGB三原色+透明度
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

		glfwSwapBuffers(window_1);					// 双缓存模式
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();								// 如果循环结束：glfw关闭
	return 0;
}
#pragma endregion

#pragma region DrawColorTriangle
GLfloat vertices_color[] =
{	// position				// color
	0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,		// 上顶点(红色)
	-0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,		// 左顶点(绿色)
	0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f		// 右顶点(蓝色)
};

//GLfloat vertices_color[] =
//{	// position				
//	0.0f, 0.5f, 0.0f,		
//	-0.5f, -0.5f, 0.0f,		
//	0.5f, -0.5f, 0.0f
//};

int DrawColorTriangle()
{
	glfwInit();								//初始化，使用glfw来打开一个窗口
	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "DrawColorTriangle", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // 获得实际占用的帧缓存大小
	glfwMakeContextCurrent(window_1);   // 捕获当前窗口，准备对当前窗口进行画图
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
		glViewport(0, 0, screenWidth, screenHeight);	// 设置视口的大小
		glDepthRange(0.01, 1000);
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// 分别是红、绿、蓝、透明度的四个参数。RGB三原色+透明度
		glClear(GL_COLOR_BUFFER_BIT);

		shader.Use();
		glBindVertexArray(VAO);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPointSize(5.0);
		glDrawArrays(GL_PATCHES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window_1);					// 双缓存模式
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();								// 如果循环结束：glfw关闭
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
	0, 1, 2,		// 序号为 0、1、3 的顶点组合成一个三角形
	0, 2, 3			// 序号为 1、2、3 的顶点组合成一个三角形
};

int DrawChangeColorRect()
{
	glfwInit();								//初始化，使用glfw来打开一个窗口
	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "DrawChangeColorRect", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // 获得实际占用的帧缓存大小
	glfwMakeContextCurrent(window_1);   // 捕获当前窗口，准备对当前窗口进行画图
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
		glViewport(0, 0, screenWidth, screenHeight);	// 设置视口的大小
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// 分别是红、绿、蓝、透明度的四个参数。RGB三原色+透明度
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

		glfwSwapBuffers(window_1);					// 双缓存模式
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();								// 如果循环结束：glfw关闭
	return 0;
}
#pragma endregion

#pragma region DrawTexture
GLfloat vertices_texture[] =
{
	//position				// texture coords(纹理坐标)
	0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		// 右上顶点		编号0
	0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		// 右下顶点		编号1
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		// 左下顶点		编号2
	-0.5f, 0.5f, 0.0f,		0.0f, 1.0f		// 左上顶点		编号3
};
int width, height;
unsigned char* image = SOIL_load_image("texture1.jpg", &width, &height, 0, SOIL_LOAD_RGBA);

GLuint indices_3[] =
{
	0, 1, 2,		// 序号为 0、1、3 的顶点组合成一个三角形
	0, 2, 3			// 序号为 1、2、3 的顶点组合成一个三角形
};

int DrawTexture()
{
	glfwInit();								//初始化，使用glfw来打开一个窗口
	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "DrawTexture", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // 获得实际占用的帧缓存大小
	glfwMakeContextCurrent(window_1);   // 捕获当前窗口，准备对当前窗口进行画图
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
		glViewport(0, 0, screenWidth, screenHeight);	// 设置视口的大小
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// 分别是红、绿、蓝、透明度的四个参数。RGB三原色+透明度
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

		glfwSwapBuffers(window_1);					// 双缓存模式
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();								// 如果循环结束：glfw关闭
	return 0;
}
#pragma endregion

#pragma region DrawCube
/* 编写各顶点位置 */
float vertices_cube[] = {
	// x、y、z 坐标				// color				// normal
	-0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,	// red 红色面
	 0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,	// green 绿色面
	 0.5f, -0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 	// blue 蓝色面(不是图中那种蓝)
	-0.5f,  0.5f, -0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,		1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,	// yellow 黄色面
	 0.5f,  0.5f, -0.5f,		1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,		1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,		1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,		1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,		1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f, 	// purple 紫色面
	 0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	-0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,	// cyan 青蓝色面
	 0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f
};


bool keys[1024];				// 专门存储按过的键
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
		cout << "按下了关闭键 esc = " << key << endl;
	}
	if (key >= 0 && key <= 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;	// true 代表按下了键
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void Key_Movement()
{
	if (keys[GLFW_KEY_Q])		// 向前
		camera.ProcessKeyboard(FORWARD, deltaTime);

	if (keys[GLFW_KEY_E])		// 向后
		camera.ProcessKeyboard(BACKWARD, deltaTime);

	if (keys[GLFW_KEY_A])		// 向左
		camera.ProcessKeyboard(LEFT, deltaTime);

	if (keys[GLFW_KEY_D])		// 向右
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (keys[GLFW_KEY_W])		// 向上
		camera.ProcessKeyboard(UPWARD, deltaTime);

	if (keys[GLFW_KEY_S])		// 向下
		camera.ProcessKeyboard(DOWNWARD, deltaTime);
}

void SquareMovement(GLfloat& up_down_move, GLfloat& left_right_move, GLfloat& front_back_move)			// 正方体移动
{
	if (keys[GLFW_KEY_UP])		// 向上
	{
		up_down_move -= Shift_pix;
	}

	if (keys[GLFW_KEY_DOWN])	// 向下
	{
		up_down_move += Shift_pix;
	}

	if (keys[GLFW_KEY_LEFT])	// 向左
	{
		left_right_move -= Shift_pix;
	}

	if (keys[GLFW_KEY_RIGHT])	// 向右
	{
		left_right_move += Shift_pix;
	}

	if (keys[GLFW_KEY_F])		// 向前(按 F 键)
	{
		front_back_move -= Shift_pix;
	}

	if (keys[GLFW_KEY_B])		// 向后(按 B 键)
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
	float yoffset = lastY - ypos;			// 注意这里是相反的

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
	glfwInit();								//初始化，使用glfw来打开一个窗口
	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "DrawCube", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // 获得实际占用的帧缓存大小
	glfwMakeContextCurrent(window_1);   // 捕获当前窗口，准备对当前窗口进行画图
	glfwSetInputMode(window_1, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// 鼠标捕获(且不显示)

	glfwSetKeyCallback(window_1, KeyCallback);						// 键盘输入
	glfwSetCursorPosCallback(window_1, mouse_callback);				// 鼠标输入
	glfwSetScrollCallback(window_1, scroll_callback);				// 滚轮输入
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

	GLfloat up_down_move = 0.0f;		// 上下移动的变量
	GLfloat left_right_move = 0.0f;		// 左右移动的变量
	GLfloat front_back_move = 0.0f;		// 前后移动的变量

	while (!glfwWindowShouldClose(window_1))
	{
		glViewport(0, 0, screenWidth, screenHeight);	// 设置视口的大小
		glfwPollEvents();
		Key_Movement();			// 获取键盘的自定义动作
		SquareMovement(up_down_move, left_right_move, front_back_move);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// 分别是红、绿、蓝、透明度的四个参数。RGB三原色+透明度
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
		transMatrix = glm::translate(transMatrix, glm::vec3(left_right_move, up_down_move, front_back_move));	// 转换矩阵
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
		// 双缓存模式
		glfwSwapBuffers(window_1);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();								// 如果循环结束：glfw关闭
	return 0;
}
#pragma endregion

#pragma region DrawDragon

int DrawDragon()
{
	glfwInit();								//初始化，使用glfw来打开一个窗口
	GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "DrawDragon", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);  // 获得实际占用的帧缓存大小
	glfwMakeContextCurrent(window_1);   // 捕获当前窗口，准备对当前窗口进行画图
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
		glViewport(0, 0, screenWidth, screenHeight);	// 设置视口的大小
		glDepthRange(0.01, 1000);
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// 分别是红、绿、蓝、透明度的四个参数。RGB三原色+透明度
		glClear(GL_COLOR_BUFFER_BIT);

		shader.Use();
		glBindVertexArray(VAO);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPointSize(5.0);
		glDrawArrays(GL_PATCHES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window_1);					// 双缓存模式
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();								// 如果循环结束：glfw关闭
	return 0;
}
#pragma endregion

int main()
{
	DrawTriangle();
}