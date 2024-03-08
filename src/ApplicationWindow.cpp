/*
* This file implements methods we need to interact with the SFML window. Specifically it handles creation and destruction
* of the window.
*/

#include "headers/Application.h"

using namespace Simplex;

void Application::InitWindow(String window_name)
{
	uint sfml_window_style_code = sf::Style::Default;

	if (system->IsWindowBorderless()) {
		sfml_window_style_code = sf::Style::None;
	}

	if (system->IsWindowFullscreen()) {
		sfml_window_style_code = sf::Style::Fullscreen;
	}

	//If OpenGL 4.5 is not supported in the system glfw will warn you and determine the highest possible version
	sfml_window = new sf::Window(sf::VideoMode(system->GetWindowWidth(), system->GetWindowHeight(), 32),
		window_name, //window name
		sfml_window_style_code, //window style
		sf::ContextSettings(24, //depth buffer
			0,	//stencil
			0,	//Antialiasing
			4,	//OpenGL Major
			5,	//OpenGL Minor
			0,	//Attributes
			false //sRGB
		));
	sfml_window->setVerticalSyncEnabled(true);
	sfml_window->setFramerateLimit(system->GetMaxFrameRate());
	sf::Image icon = file_system->LoadImageFromResource("iconPNG");
	sfml_window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);	// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);		// The Type Of Depth Testing To Do

	glEnable(GL_CULL_FACE);
}

void Application::Display(void)
{
	ClearScreen();

	mesh_manager->AddSkyboxToRenderList();

	render_call_per_frame_count = mesh_manager->Render();

	mesh_manager->ClearRenderList();

	DrawGUI();

	// End the current frame (internally swaps the front and back buffers)
	sfml_window->display();
}

void Application::ClearScreen(vector4 clear_color)
{
	if (clear_color != vector4(-1.0f)) {
		background_color = clear_color;
	}
	glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Application::ResizeWindow(void)
{
	sf::Vector2i pos = sfml_window->getPosition();
	sf::Vector2u size = sfml_window->getSize();

	system->SetWindowX(pos.x);
	system->SetWindowY(pos.y);
	system->SetWindowWidth(size.x);
	system->SetWindowHeight(size.y);

	// Adjust the viewport when the window is resized
	glViewport(0, 0, size.x, size.y);
	mesh_manager->SetRenderTarget(0, 0, 0, size.x, size.y);
}
