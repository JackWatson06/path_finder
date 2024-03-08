/*
* Define a generic ImGUIObject which handles the graphical information coming from ImGUI so we can render it with OpenGL.
*/
#pragma once

#include "imgui\imgui.h"
#include "GL\glew.h"

struct ImGuiObject
{
	ImGuiIO& io = ImGui::GetIO(); //IO from ImGui

	int m_nShader = 0; //Shader handler
	int m_nTex = 0; //Text
	int m_nProjMtx = 0; //Projection Matrix
	unsigned int m_uVBO = 0; //Vertex Buffered Object
	unsigned int m_uVAO = 0; //Vertex Array Object
	unsigned int m_uElements = 0; //elements handle

	int m_nVertHandle = 0; //Vertex Shader
	int m_nFragHandle = 0; //Fragment Shader

	int m_nPosition = 0; //Attribute location for position
	int m_nUV = 0; //Attribute location for UV
	int m_nColor = 0; //Attribute location for color

	unsigned int clock; //Clock to use for the gui
	double m_dTimeTotal = 0.0f; //Time
	bool mouse_input[3] = { false, false, false }; //flags for mouse
	GLuint m_uFontTexture = 0; //Font texture
};