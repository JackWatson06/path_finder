/*
* Handle updating the main simulation state and events. This file will take in input through events. Update the game
* state. Then render that state out to the monitor.
*/
#include "headers/Application.h"

using namespace Simplex;
void Application::InitGameState(void)
{
	camera_manager->SetPositionTargetAndUpward(
		vector3(10.0f, 9.0f, 26.0f), //Position
		vector3(10.0f, -8.0f, 0.0f), //Target
		AXIS_Y);					//Up

	light_manager->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1);
	a_star_simulation = AStarSimulation::GetInstance();
}

void Application::Run(void)
{
	Init();

	is_app_running = true;
	sf::Vector2i pos = sfml_window->getPosition();
	sf::Vector2u size = sfml_window->getSize();
	while (is_app_running) {

		sf::Event appEvent;
		while (sfml_window->pollEvent(appEvent)) {
			
			switch (appEvent.type)
			{
				case sf::Event::Closed:
					is_app_running = false;
					break;
				case sf::Event::Resized:
					size = sfml_window->getSize();
					system->SetWindowWidth(size.x);
					system->SetWindowHeight(size.y);
					ResizeWindow();
					break;
				case sf::Event::MouseMoved:
					ProcessMouseMovement();
					if (pos != sfml_window->getPosition())
					{
						pos = sfml_window->getPosition();
						size = sfml_window->getSize();

						system->SetWindowX(pos.x);
						system->SetWindowY(pos.y);
						system->SetWindowWidth(size.x);
						system->SetWindowHeight(size.y);
					}
					break;
				case sf::Event::MouseButtonPressed:
					ProcessMousePressed(appEvent);
					break;
				case sf::Event::MouseButtonReleased:
					ProcessMouseReleased(appEvent);
					break;
				case sf::Event::MouseWheelScrolled:
					ProcessMouseScroll(appEvent);
					break;
				case sf::Event::KeyPressed:
					ProcessKeyPressed(appEvent);
					break;
				case sf::Event::KeyReleased:
					ProcessKeyReleased(appEvent);
					break;
				case sf::Event::TextEntered:
					if (appEvent.text.unicode > 0 && appEvent.text.unicode < 0x10000)
						ImGui::GetIO().AddInputCharacter(appEvent.text.unicode);
					break;
				case sf::Event::JoystickButtonPressed:
					ProcessJoystickPressed(appEvent);
					break;
				case sf::Event::JoystickButtonReleased:
					ProcessJoystickReleased(appEvent);
					break;
				case sf::Event::JoystickMoved:
					ProcessJoystickMoved(appEvent);
					break;
				case sf::Event::JoystickConnected:
					ProcessJoystickConnected(appEvent.joystickConnect.joystickId);
					break;
				case sf::Event::JoystickDisconnected:
					InitControllers();
					break;
				case sf::Event::GainedFocus:
					is_main_window_focused = true;
					break;
				case sf::Event::LostFocus:
					is_main_window_focused = false;
					break;
			}
		}
		ProcessKeyboard();
		ProcessJoystick();
		Update();
		Display();
	}

	WriteConfig();
}

void Application::Update(void)
{
	system->Update();

	// Update mouse pointer state depending on if we have arcball or first person camera enabled.
	ArcBall();
	CameraRotation();

	// Update entities in the scene and our simulation.
	entity_manager->Update();
	a_star_simulation->Update();

	// Add entities for next render.
	entity_manager->AddEntityToRenderList(-1, true);
	a_star_simulation->AddLinesToRenderList();
}
