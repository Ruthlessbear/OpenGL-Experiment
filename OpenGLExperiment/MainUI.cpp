#include <iostream>
using namespace std;

#include <GL/glew.h>    
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"glm/gtx/rotate_vector.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <random>
#include <map>

namespace OPUI
{
	int UI_WIDTH = 300;
	int UI_HEIGHT = 400;

	//define parameter
	float* testf = 0;

	char* buf = "a";
	float f;

	enum ParameterType
	{
		Float = 0,
		Vec2,
		Vec3,
		Vec4
	};

	class ParameterElement
	{
	public:
		ParameterType type;
		string parameter_name;
		float parameter_value[4];

		ParameterElement(OPUI::ParameterType _type, string _name, float* _values, int size)
		{
			type = _type;
			parameter_name = _name;
			memcpy(parameter_value, _values, size * sizeof(float));
		}
	};

#pragma region Parameter Func
	std::map<std::string, ParameterElement> parameter_maps;

	float* SetParamFloat(string name, float value)
	{
		std::map<string, ParameterElement>::iterator target = parameter_maps.find(name);
		float values[] = { value };
		if (target == parameter_maps.end())
		{
			cout << "create parameter " << name << " value is " << value << std::endl;
			ParameterType type = ParameterType::Float;
			ParameterElement para(type, name, values, 1);
			parameter_maps.insert(pair<std::string, ParameterElement>(name, para));
			return &(para.parameter_value[0]);
		}
		else
		{
			ParameterElement* para = &(*target).second;
			return &((*para).parameter_value[0]);
		}
	}

	float* SetParamVec2(string name, ImVec2 value)
	{
		std::map<string, ParameterElement>::iterator target = parameter_maps.find(name);
		float values[2] = { value.x, value.y};
		if (target == parameter_maps.end())
		{
			cout << "create parameter " << name << " value is " << value.x << "," << value.y << std::endl;
			ParameterType type = ParameterType::Vec2;
			ParameterElement para(type, name, values, 2);
			parameter_maps.insert(pair<std::string, ParameterElement>(name, para));
			return &(para.parameter_value[0]);
		}
		else
		{
			ParameterElement* para = &(*target).second;
			return &((*para).parameter_value[0]);
		}
	}

	float* SetParamVec4(string name, ImVec4 value)
	{
		std::map<string, ParameterElement>::iterator target = parameter_maps.find(name);
		float values[4] = { value.x, value.y, value.z, value.w};
		if (target == parameter_maps.end())
		{
			cout << "create parameter " << name << " value is " << value.x << "," << value.y << "," << value.z << "," << value.w << std::endl;
			ParameterType type = ParameterType::Vec4;
			ParameterElement para(type, name, values, 4);
			parameter_maps.insert(pair<std::string, ParameterElement>(name, para));
			return &(para.parameter_value[0]);
		}
		else
		{
			ParameterElement* para = &(*target).second;
			return &((*para).parameter_value[0]);
		}
	}

	float* GetParamFloat(string name)
	{
		std::map<std::string, ParameterElement>::iterator target = parameter_maps.find(name);
		if (target != parameter_maps.end())
		{
			ParameterElement* para = &(*target).second;
			if (sizeof((*para).parameter_value) <= ParameterType::Float)
				cout << "Error save in parameter dic: " << name << "->" << ParameterType::Float << endl;
			
			return &((*para).parameter_value[0]);
		}
		else
		{
			float define_zero = 0.0f;
			return &define_zero;
		}
	}

	float GetUIFloat(string name)
	{
		return *(GetParamFloat(name));
	}

	float* GetParamVec2(string name)
	{
		std::map<std::string, ParameterElement>::iterator target = parameter_maps.find(name);
		if (target != parameter_maps.end())
		{
			ParameterElement* para = &(*target).second;
			if (sizeof((*para).parameter_value) <= ParameterType::Vec2)
				cout << "Error save in parameter dic: " << name << "->" << ParameterType::Vec2 << endl;

			return &((*para).parameter_value[0]);
		}
		else
		{
			float define_zero = 0.0f;
			return &define_zero;
		}
	}

	glm::vec2 GetUIVec2(string name)
	{
		float* result = GetParamVec2(name);
		return glm::vec2(*(result), *(result + 1));
	}

	float* GetParamVec4(string name)
	{
		std::map<std::string, ParameterElement>::iterator target = parameter_maps.find(name);
		if (target != parameter_maps.end())
		{
			ParameterElement* para = &(*target).second;
			if (sizeof((*para).parameter_value) <= ParameterType::Vec4)
				cout << "Error save in parameter dic: " << name << "->" << ParameterType::Vec4 << endl;

			return &((*para).parameter_value[0]);
		}
		else
		{
			float define_zero = 0.0f;
			return &define_zero;
		}
	}

	glm::vec4 GetUIVec4(string name)
	{
		float* result = GetParamVec4(name);
		return glm::vec4(*(result), *(result + 1), *(result + 2), *(result + 3));
	}

#pragma endregion

	void DrawMainUI()
	{
		//initialize
		int UI_WIDTH = 200;
		ImGui::SetWindowSize(ImVec2(UI_WIDTH, UI_HEIGHT), 0);

		ImGui::Text("Hello, Test UI %d", 123);

		float* test_float = SetParamFloat("test_float", 1.0f);
		ImGui::SliderFloat("ssao_factor", test_float, 0.0f, 1.0f);

		float* test_color = SetParamVec4("test_color", ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::ColorEdit4("ssao_color", test_color);
	}

	bool cursor_enable = false;
	float cursor_change_time = 0.0f;
	bool cursor_wait_enable = true;
	void KeyBoardIn(GLFWwindow* window, float deltaTime)
	{
		if (!cursor_wait_enable)
		{
			cursor_change_time += deltaTime;
			if (cursor_change_time > 0.5f)
			{
				cursor_change_time = 0.0f;
				cursor_wait_enable = true;
			}
		}
		//mouse enable switch
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			if (!cursor_wait_enable) return;
			if (!cursor_enable)
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			else
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cursor_wait_enable = false;
			cursor_enable = !cursor_enable;
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}
}
