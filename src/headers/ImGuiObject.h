#pragma once

#include "imgui\imgui.h"

struct ImGuiObject
{
	ImGuiIO& io = ImGui::GetIO();

	int m_nShader = 0;
	int m_nTex = 0;
	int m_nProjMtx = 0;
	unsigned int m_uVBO = 0;
	unsigned int m_uVAO = 0;
	unsigned int m_uElements = 0;

	int m_nVertHandle = 0;
	int m_nFragHandle = 0;

	int m_nPosition = 0;
	int m_nUV = 0;
	int m_nColor = 0;

	unsigned int m_nClock;
	double m_dTimeTotal = 0.0f;
	bool m_bMousePressed[3] = { false, false, false };
	GLuint m_uFontTexture = 0;
};