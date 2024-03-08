/*
* This file implements several of the methods needed to handle input into our application. Our application allows
* for mouse control, keyboard control, and use of a gamepad.
*/

#include "headers/Application.h"

using namespace Simplex;

void Application::ProcessMouseMovement()
{
	sf::Vector2i mouse = sf::Mouse::getPosition();
	sf::Vector2i window = sfml_window->getPosition();
	mouse_position.x = static_cast<float>(mouse.x - window.x);
	mouse_position.y = static_cast<float>(mouse.y - window.y);
	if (!system->IsWindowFullscreen() && !system->IsWindowBorderless())
		mouse_position += vector3(-8.0f, -32.0f, 0.0f);
	gui.io.MousePos = ImVec2(mouse_position.x, mouse_position.y);
}

void Application::ProcessMousePressed(sf::Event sfml_event)
{
	switch (sfml_event.mouseButton.button)
	{
		case sf::Mouse::Button::Left:
			gui.mouse_input[0] = true;
			break;
		case sf::Mouse::Button::Middle:
			gui.mouse_input[1] = true;
			is_arc_ball_toggled = true;
			break;
		case sf::Mouse::Button::Right:
			gui.mouse_input[2] = true;
			is_first_person_camera_toggled = true;
			break;
		default: break;
	}

	for (int i = 0; i < 3; i++) {
		gui.io.MouseDown[i] = gui.mouse_input[i];
	}
}

void Application::ProcessMouseReleased(sf::Event sfml_event)
{
	switch (sfml_event.mouseButton.button)
	{
		case sf::Mouse::Button::Left:
			gui.mouse_input[0] = false;
			break;
		case sf::Mouse::Button::Middle:
			gui.mouse_input[1] = false;
			is_arc_ball_toggled = false;
			break;
		case sf::Mouse::Button::Right:
			gui.mouse_input[2] = false;
			is_first_person_camera_toggled = false;
			break;
		default: break;
	}

	for (int i = 0; i < 3; i++) {
		gui.io.MouseDown[i] = gui.mouse_input[i];
	}
}

void Application::ProcessMouseScroll(sf::Event sfml_event)
{
	gui.io.MouseWheel = sfml_event.mouseWheelScroll.delta;
	float fSpeed = sfml_event.mouseWheelScroll.delta;
	float fMultiplier = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

	if (fMultiplier) {
		fSpeed *= 2.0f;
	}
	camera_manager->MoveForward(-fSpeed);
}

void Application::ProcessKeyPressed(sf::Event sfml_event)
{
	switch (sfml_event.key.code)
	{
		case sf::Keyboard::Space:
			sound_effect.play();
			entity_manager->ApplyForce(vector3(0.0f, 1.0f, 0.0f), "Steve");
			break;
		case sf::Keyboard::LShift:
		case sf::Keyboard::RShift:
			shift_modifier_key_pressed = true;
			break;
		default: break;
	}

	gui.io.KeysDown[sfml_event.key.code] = true;
	gui.io.KeyCtrl = sfml_event.key.control;
	gui.io.KeyShift = sfml_event.key.shift;
}

void Application::ProcessKeyReleased(sf::Event sfml_event)
{
	static bool toggl_FPS_control = false;

	switch (sfml_event.key.code)
	{
		case sf::Keyboard::Escape:
			is_app_running = false;
			break;
		case sf::Keyboard::F1:
			camera_manager->SetCameraMode(CAM_PERSP);
			break;
		case sf::Keyboard::F2:
			camera_manager->SetCameraMode(CAM_ORTHO_Z);
			break;
		case sf::Keyboard::F3:
			camera_manager->SetCameraMode(CAM_ORTHO_Y);
			break;
		case sf::Keyboard::F4:
			camera_manager->SetCameraMode(CAM_ORTHO_X);
			break;
		case sf::Keyboard::F:
			toggl_FPS_control = !toggl_FPS_control;
			camera_manager->SetFPS(toggl_FPS_control);
			break;
		case sf::Keyboard::PageUp:
			break;
		case sf::Keyboard::PageDown:
			break;
		case sf::Keyboard::Add:
			a_star_simulation->AddObstacle();
			break;
		case sf::Keyboard::Subtract:
			a_star_simulation->RemoveObstacle();
			break;
		case sf::Keyboard::LShift:
		case sf::Keyboard::RShift:
			shift_modifier_key_pressed = false;
		default: break;
	}

	gui.io.KeysDown[sfml_event.key.code] = false;
	gui.io.KeyCtrl = sfml_event.key.control;
	gui.io.KeyShift = sfml_event.key.shift;
}

void Application::ProcessJoystickConnected(uint controller_id)
{
	// We only allow 8 controller inputs.
	if (controller_id > 7) {
		return;
	}

	bool controller_connected = sf::Joystick::isConnected(controller_id);
	show_GUI_controller_window = controller_connected;
	if (controller_connected)
	{
		SafeDelete(controller_input[controller_id]);
		sf::Joystick::Identification joyID = sf::Joystick::getIdentification(controller_id);
		String productName = joyID.name.toAnsiString();
		int nVendorID = joyID.vendorId;
		int nProductID = joyID.productId;
		controller_input[controller_id] = new ControllerInput(nVendorID, nProductID);
		++connected_controller_count;
	}
}
void Application::ProcessJoystickPressed(sf::Event sfml_event)
{
	int nID = sfml_event.joystickButton.joystickId;
	int nButton = sfml_event.joystickButton.button;
	controller_input[nID]->button[controller_input[nID]->mapButton[nButton]] = true;
	if (controller_input[active_controller_id]->button[SimplexKey_L3] && controller_input[active_controller_id]->button[SimplexKey_R3]) {
		is_app_running = false;
	}
}
void Application::ProcessJoystickReleased(sf::Event sfml_event)
{
	int nID = sfml_event.joystickButton.joystickId;
	int nButton = sfml_event.joystickButton.button;
	controller_input[nID]->button[controller_input[nID]->mapButton[nButton]] = false;

	if (controller_input[active_controller_id]->mapButton[nButton] == SimplexKey_Pad) {
		is_app_running = false;
	}
}
void Application::ProcessJoystickMoved(sf::Event sfml_event)
{
	int nID = sfml_event.joystickMove.joystickId;
	float fPosition = sfml_event.joystickMove.position;
	int nAxis = sfml_event.joystickMove.axis;

	//Invert vertical axis for sticks
	if (nAxis == sf::Joystick::Axis::Y || nAxis == sf::Joystick::Axis::R) {
		fPosition *= -1.0f;
	}

	// Adjust with threshold
	float fThreshold = 10.0f;
	if (nAxis != sf::Joystick::Axis::PovX || nAxis != sf::Joystick::Axis::PovY) {
		if (fPosition > -fThreshold && fPosition < fThreshold) {
			fPosition = 0.0f;
		}
	}

	/*
	SFML does not use XInput so XBOX one controller is a special snowflake,
	LTrigger and RTrigger share the same axis (sf::Joystick::Axis::Z)
	instead of separate ones as	in other controllers:
	LTrigger reports from 0 to 100 and RTrigger reports from 0 to -100
	I mapped the values from 0 to 100 in both cases to keep consistent
	with other controllers, but sharing the axis means that triggers cannot
	be independent from each other
	*/
	//PS4 Controller
	if (controller_input[nID]->uModel == SimplexController_DualShock4) {
		//For Axis U or V
		if (nAxis == sf::Joystick::Axis::U || nAxis == sf::Joystick::Axis::V) {
			fPosition = MapValue(fPosition, -100.0f, 100.0f, 0.0f, 100.0f);
		}
		controller_input[nID]->axis[controller_input[nID]->mapAxis[nAxis]] = fPosition;
	//Nintendo Switch Pro
	} else if (controller_input[nID]->uModel == SimplexController_SwitchPro) {
		if (nAxis != sf::Joystick::Axis::PovX && nAxis != sf::Joystick::Axis::PovY)
		{
			fPosition = MapValue(fPosition, -75.0f, 75.0f, -100.0f, 100.0f);
			if (fPosition > 100) fPosition = 100;
			if (fPosition < -100) fPosition = -100;
			if (fPosition > -10 && fPosition < 10) fPosition = 0;
		}
		controller_input[nID]->axis[controller_input[nID]->mapAxis[nAxis]] = fPosition;
	//Microsoft controllers
	} else if (controller_input[nID]->uModel == SimplexController_XBONE || controller_input[nID]->uModel == SimplexController_360) {
		//Axis different than Z
		if (nAxis != sf::Joystick::Axis::Z) {
			controller_input[nID]->axis[controller_input[nID]->mapAxis[nAxis]] = fPosition;
		//Z Axis
		} else {
			fPosition = sfml_event.joystickMove.position;
			
			//positive values go on the left trigger
			if (fPosition >= 0) {
				if (fPosition < 25.0f) {
					fPosition = 0.0f;
				}
				controller_input[nID]->axis[SimplexAxis_L] = fPosition;
			//negative values go on the right trigger
			} else {
				if (fPosition > -25.0f) {
					fPosition = 0.0f;
				}
				controller_input[nID]->axis[SimplexAxis_R] = -fPosition;
			}

		}
	//8Bitdo NES30 PRO
	} else if (controller_input[nID]->uModel == SimplexController_NES30PRO) {
		controller_input[nID]->axis[controller_input[nID]->mapAxis[nAxis]] = fPosition;
	//Other controllers
	} else {
		controller_input[nID]->axis[controller_input[nID]->mapAxis[nAxis]] = fPosition;
	}
}

void Application::ProcessKeyboard(void)
{
	if (!is_main_window_focused) {
		return;
	}
	/*
	This is used for things that are continuously happening,
	for discreet on/off use ProcessKeyboardPressed/Released
	*/
	bool bMultiplier = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

	float fMultiplier = 1.0f;

	if (bMultiplier)
		fMultiplier = 5.0f;

#pragma region Camera Position
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		camera_manager->MoveForward(camera_movement_speed * fMultiplier);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		camera_manager->MoveForward(-camera_movement_speed * fMultiplier);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		camera_manager->MoveSideways(-camera_movement_speed * fMultiplier);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		camera_manager->MoveSideways(camera_movement_speed * fMultiplier);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
		camera_manager->MoveVertical(-camera_movement_speed * fMultiplier);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
		camera_manager->MoveVertical(camera_movement_speed * fMultiplier);
	}

#pragma endregion

#pragma region Character Position
	float fDelta = system->GetDeltaTime(0);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		entity_manager->ApplyForce(vector3(-2.0f * fDelta, 0.0f, 0.0f), "Steve");
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		entity_manager->ApplyForce(vector3(2.0f * fDelta, 0.0f, 0.0f), "Steve");
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		entity_manager->ApplyForce(vector3(0.0f, 0.0f, -2.0f * fDelta), "Steve");
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		entity_manager->ApplyForce(vector3(0.0f, 0.0f, 2.0f * fDelta), "Steve");
	}

#pragma endregion
}

void Application::ProcessJoystick(void)
{
	if (!is_main_window_focused) {
		return;
	}

	/*
	This is used for things that are continuously happening,
	for discreet on/off use ProcessJoystickPressed/Released
	*/
#pragma region Camera Position
	float fForwardSpeed = controller_input[active_controller_id]->axis[SimplexAxis_Y] / 150.0f;
	float fHorizontalSpeed = controller_input[active_controller_id]->axis[SimplexAxis_X] / 150.0f;
	float fVerticalSpeed = controller_input[active_controller_id]->axis[SimplexAxis_R] / 150.0f -
		controller_input[active_controller_id]->axis[SimplexAxis_L] / 150.0f;

	bool fMultiplier = controller_input[active_controller_id]->button[SimplexKey_L1] ||
		controller_input[active_controller_id]->button[SimplexKey_R1];

	if (fMultiplier) {
		fForwardSpeed *= 3.0f;
		fHorizontalSpeed *= 3.0f;
		fVerticalSpeed *= 3.0f;
	}

	camera_manager->MoveForward(fForwardSpeed);
	camera_manager->MoveSideways(fHorizontalSpeed);
	camera_manager->MoveVertical(fVerticalSpeed);
#pragma endregion

#pragma region Camera Orientation
	//Change the Yaw and the Pitch of the camera
	float fUSpeed = glm::radians(controller_input[active_controller_id]->axis[SimplexAxis_U] / 150.0f);
	float fVSpeed = glm::radians(controller_input[active_controller_id]->axis[SimplexAxis_V] / 150.0f);
	camera_manager->ChangeYaw(-fUSpeed);
	camera_manager->ChangePitch(fVSpeed);
#pragma endregion

#pragma region ModelOrientation Orientation
	arc_ball_orientation = quaternion(vector3(glm::radians(controller_input[active_controller_id]->axis[SimplexAxis_POVY] / 20.0f), 0.0f, 0.0f)) * arc_ball_orientation;
	if (fMultiplier) {
		arc_ball_orientation = quaternion(vector3(0.0f, 0.0f, glm::radians(controller_input[active_controller_id]->axis[SimplexAxis_POVX] / 20.0f))) * arc_ball_orientation;
	} else {
		arc_ball_orientation = quaternion(vector3(0.0f, glm::radians(controller_input[active_controller_id]->axis[SimplexAxis_POVX] / 20.0f), 0.0f)) * arc_ball_orientation;
	}
#pragma endregion
}

void Application::InitControllers(void)
{
	//Make sure this method is only executed once
	//else release the controllers first
	static bool bInit = false;
	if (bInit) ReleaseControllers();

	//Init all pointers
	for (uint i = 0; i < 8; ++i) {
		controller_input[i] = new ControllerInput();
	}

	active_controller_id = 0;
	connected_controller_count = 0;
	for (uint i = 0; i < 8; ++i) {
		if (sf::Joystick::isConnected(i)) {
			ProcessJoystickConnected(i);
		}
	}
	bInit = true;
}

void Application::ReleaseControllers(void)
{
	for (uint i = 0; i < 8; ++i) {
		if (controller_input[i] != nullptr) {
			delete controller_input[i];
			controller_input[i] = nullptr;
		}
	}
	connected_controller_count = 0;
}
