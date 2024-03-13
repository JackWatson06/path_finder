/*
* This header file contains all of the definitions which model our game world and manage OpenGL. 
*/
#pragma once

#include "./ControllerConfiguration.h"
#include "./FileSystem.h"
#include "./GameEntityManager.h"
#include "./AStarSimulation.h"
#include "./ImGuiObject.h"

#include "Simplex\Simplex.h"
#include "SFML\Window.hpp"
#include "SFML\Graphics.hpp"
#include "SFML\OpenGL.hpp"
#include "SFML\Audio.hpp"

namespace Simplex
{
	class Application
	{	
		private:
		# pragma region Window
			//vector4 background_color;
			vector4 background_color;
			sf::Window* sfml_window = nullptr;
		# pragma endregion

		# pragma region GUI
			static ImGuiObject gui;
			bool show_GUI_main_window = true;
			bool show_GUI_console_window = true;
			bool show_GUI_test_window = false;
			bool show_GUI_controller_window = false;
		# pragma endregion
			
		# pragma region Camera			
			vector3 mouse_position = vector3();
			bool is_main_window_focused = true;
			bool is_arc_ball_toggled = false;
			quaternion arc_ball_orientation;
			bool is_first_person_camera_toggled = false;
			float camera_movement_speed = 0.1f;
		# pragma endregion
			
		# pragma region Game State
			bool is_app_running = false;
			uint render_call_per_frame_count = 0;
			SystemSingleton* system = nullptr;
			LightManager* light_manager = nullptr;
			MeshManager* mesh_manager = nullptr;
			CameraManager* camera_manager = nullptr;
			AStarSimulation* a_star_simulation = nullptr;
			GameEntityManager* entity_manager = nullptr;
		# pragma endregion
			
		# pragma region Controller
			uint connected_controller_count = 0;
			ControllerInput* controller_input[8];
			uint active_controller_id = 0;
			bool shift_modifier_key_pressed = false;
		# pragma endregion

		# pragma region FileSystem
			FileSystem* file_system;

		# pragma region Sound
			sf::SoundBuffer sound_buffer;
			sf::Sound sound_effect;
			sf::Music background_music;
		# pragma endregion

		public:

		#pragma region Init
			Application();
			void Init(String application_name = "", int resolution = BTO_RESOLUTIONS::RES_C_1280x720_16x9_HD,
				bool fullscreen = false, bool borderless= false);
			void Init(String application_name, uint window_width, uint window_height, bool fullscreen, bool borderless);
			~Application(void);
		#pragma endregion

			void Run(void);
		private:
		#pragma region Init
			Application(Application const& input);
			Application& operator=(Application const& input);
			void Release(void);
		#pragma endregion
			
		#pragma region Settings
			void ReadConfig(void);
			void WriteConfig(void);
		#pragma endregion

		#pragma region Window
			void InitWindow(String window_name = "Application");
			void Display(void);
			void ClearScreen(vector4 clear_color = vector4(-1.0f));
			void ResizeWindow(void);
		#pragma endregion		

		#pragma region GUI
			void InitIMGUI(void);
			void DrawGUI(void);
			void ShutdownGUI(void);
			bool CreateFontsTexture(void);
			bool CreateDeviceObjects(void);
			static void RenderDrawLists(ImDrawData* draw_data);
			void NewFrame(void);
		#pragma endregion

		#pragma region Camera
			void ArcBall(float sensitivity = 0.1f);
			void CameraRotation(float a_fSpeed = 0.005f);
		#pragma endregion

		#pragma region Game State
			void InitGameState(void);
			void Update(void);
		#pragma endregion

		#pragma region Controllers
			void InitControllers(void);
			void ReleaseControllers(void);
			void ProcessKeyboard(void);
			void ProcessJoystick(void);
			void ProcessKeyPressed(sf::Event sfml_event);
			void ProcessKeyReleased(sf::Event sfml_event);
			void ProcessMouseMovement();
			void ProcessMousePressed(sf::Event sfml_event);
			void ProcessMouseReleased(sf::Event sfml_event);
			void ProcessMouseScroll(sf::Event sfml_event);
			void ProcessJoystickConnected(uint controller_id = 0);
			void ProcessJoystickPressed(sf::Event sfml_event);
			void ProcessJoystickReleased(sf::Event sfml_event);
			void ProcessJoystickMoved(sf::Event sfml_event);
		#pragma endregion
	};

}