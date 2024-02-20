#pragma once
#include <GL/glew.h>    
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace OPUI
{
	void DrawMainUI();
	void KeyBoardIn(GLFWwindow* window, float deltaTime);
	//--------set parameter-------
	float* SetParamFloat(string name, float value);
	float* SetParamVec2(string name, ImVec2 value);
	float* SetParamVec4(string name, ImVec4 value);
	//--------get parameter-------
	float* GetParamFloat(string name);
	float* GetParamVec2(string name);
	float* GetParamVec4(string name);
	float GetUIFloat(string name);
	glm::vec2 GetUIVec2(string name);
	glm::vec4 GetUIVec4(string name);

}

