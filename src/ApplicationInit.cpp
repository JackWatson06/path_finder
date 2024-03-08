/*
* Initialization and destruction of our main application class. We start up several different services here which help
* running the A Star simulation.
*/
#include "./headers/Application.h"

using namespace Simplex;

Application::Application() {}
Application::Application(Application const& input) {}
Application& Application::operator=(Application const& input) { return *this; }

Application::~Application(void) 
{
	Release();
	ReleaseControllers();
	Simplex::ReleaseAllSingletons();
	SafeDelete(sfml_window);
}

void Application::Release(void)
{
	GameEntityManager::ReleaseInstance();
	AStarSimulation::ReleaseInstance();
	ShutdownGUI();
}

void Application::Init(String application_name, int resolution, bool fullscreen, bool borderless)
{
	static bool application_initialized = false;
	if (application_initialized) {
		return;
	}

	//Init System
	system = SystemSingleton::GetInstance();
	system->SetWindowResolution(resolution);
	Init(application_name, system->GetWindowWidth(), system->GetWindowHeight(), fullscreen, borderless);

	//Initializated flag
	application_initialized = true;
}

void Application::Init(String application_name, uint window_with, uint window_height, bool fullscreen, bool borderless)
{
	static bool application_initialized = false;
	if (application_initialized) {
		return;
	}

	is_app_running = false;

	system = SystemSingleton::GetInstance();

	if (application_name == "") {
		system->SetWindowName(system->GetAppName());
	}
	else {
		system->SetWindowName(application_name);
	}

	system->SetWindowWidth(window_with);
	system->SetWindowHeight(window_height);
	system->SetWindowFullscreen(fullscreen);

	light_manager = LightManager::GetInstance();
	background_color = vector4(C_BLUE_CORNFLOWER, 1.0f);

	file_system = FileSystem::GetInstance();

	ReadConfig(); // The .ini file will have preference over programmer 
	
	InitWindow(system->GetWindowName());

	camera_manager = CameraManager::GetInstance();
	camera_manager->SetPositionTargetAndUpward(
		vector3(0.0f, 2.5f, 15.0f),//Camera position
		vector3(0.0f, 2.5f, 0.0f),//What I'm looking at
		AXIS_Y);//What is up

	mesh_manager = MeshManager::GetInstance();

	ResizeWindow();

	show_GUI_main_window = true;
	show_GUI_console_window = true;
	show_GUI_controller_window = false;
	show_GUI_test_window = false;
	InitIMGUI();

	InitControllers();

	entity_manager = GameEntityManager::GetInstance();

	InitGameState();

	application_initialized = true;
}
