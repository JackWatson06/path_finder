/*
* The FileSystem header defines functions that are used to interact with the underlying file system to load textures,
* images, or any file data.
*/
#pragma once

#include "Simplex/Simplex.h"
#include "SFML/Graphics.hpp"

namespace Simplex {
	class FileSystem 
	{
		private:
			static FileSystem* instance;

			FileSystem();
			void Init();
		public:
			static FileSystem* GetInstance();
			static void ReleaseInstance();

			sf::Image LoadImageFromResource(const std::string& name);
	};
}
