/*
* Handle movement of the camera through a first person view or the arcball.
*/

# include "headers/Application.h"

using namespace Simplex;

void Application::ArcBall(float a_fSensitivity)
{
	if (!is_arc_ball_toggled)
		return;

	UINT	MouseX, MouseY;
	UINT	CenterX, CenterY;

	//Initialize the position of the pointer to the middle of the screen
	CenterX = system->GetWindowX() + system->GetWindowWidth() / 2;
	CenterY = system->GetWindowY() + system->GetWindowHeight() / 2;

	//Calculate the position of the mouse and store it
	POINT pt;
	GetCursorPos(&pt);
	MouseX = pt.x;
	MouseY = pt.y;

	//Calculate the difference in position and update the quaternion orientation based on it
	float DeltaMouse;
	if (MouseX < CenterX)
	{
		DeltaMouse = static_cast<float>(CenterX - MouseX);
		arc_ball_orientation = quaternion(vector3(0.0f, glm::radians(a_fSensitivity * DeltaMouse), 0.0f)) * arc_ball_orientation;
	}
	else if (MouseX > CenterX)
	{
		DeltaMouse = static_cast<float>(MouseX - CenterX);
		arc_ball_orientation = quaternion(vector3(0.0f, glm::radians(-a_fSensitivity * DeltaMouse), 0.0f)) * arc_ball_orientation;
	}

	if (MouseY < CenterY)
	{
		DeltaMouse = static_cast<float>(CenterY - MouseY);
		arc_ball_orientation = quaternion(vector3(glm::radians(-a_fSensitivity * DeltaMouse), 0.0f, 0.0f)) * arc_ball_orientation;
	}
	else if (MouseY > CenterY)
	{
		DeltaMouse = static_cast<float>(MouseY - CenterY);
		arc_ball_orientation = quaternion(vector3(glm::radians(a_fSensitivity * DeltaMouse), 0.0f, 0.0f)) * arc_ball_orientation;
	}

	SetCursorPos(CenterX, CenterY);
}

void Application::CameraRotation(float a_fSpeed)
{
	if (is_first_person_camera_toggled == false)
		return;

	UINT	MouseX, MouseY;
	UINT	CenterX, CenterY;

	CenterX = system->GetWindowX() + system->GetWindowWidth() / 2;
	CenterY = system->GetWindowY() + system->GetWindowHeight() / 2;

	//Calculate the position of the mouse and store it.
	POINT pt;
	GetCursorPos(&pt);
	MouseX = pt.x;
	MouseY = pt.y;

	//Calculate the difference in view with the angle
	float fAngleX = 0.0f;
	float fAngleY = 0.0f;
	float fDeltaMouse = 0.0f;
	if (MouseX < CenterX)
	{
		fDeltaMouse = static_cast<float>(CenterX - MouseX);
		fAngleY += fDeltaMouse * a_fSpeed;
	}
	else if (MouseX > CenterX)
	{
		fDeltaMouse = static_cast<float>(MouseX - CenterX);
		fAngleY -= fDeltaMouse * a_fSpeed;
	}

	if (MouseY < CenterY)
	{
		fDeltaMouse = static_cast<float>(CenterY - MouseY);
		fAngleX -= fDeltaMouse * a_fSpeed;
	}
	else if (MouseY > CenterY)
	{
		fDeltaMouse = static_cast<float>(MouseY - CenterY);
		fAngleX += fDeltaMouse * a_fSpeed;
	}

	//Change the Yaw and the Pitch of the camera
	camera_manager->ChangeYaw(fAngleY * 0.25f);
	camera_manager->ChangePitch(-fAngleX * 0.25f);
	SetCursorPos(CenterX, CenterY);
}
