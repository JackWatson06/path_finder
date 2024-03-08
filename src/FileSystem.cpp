/*
* This file implements methods found in the FileSyste.h header which pertains to interactions with the systems
* underlying file system.
*/

#include "headers/FileSystem.h"

using namespace Simplex;

Simplex::FileSystem* Simplex::FileSystem::instance = nullptr;
Simplex::FileSystem* Simplex::FileSystem::GetInstance() 
{
	if (instance == nullptr) {
		instance = new FileSystem();
	}

	return instance;
}

void Simplex::FileSystem::ReleaseInstance() 
{
	if (instance == nullptr) {
		return;
	}

	delete instance;
	instance = nullptr;
}

sf::Image FileSystem::LoadImageFromResource(const std::string& name)
{
	const size_t name_length = name.size() + 1;
	wchar_t* unicode_name = new wchar_t[name_length];
	mbstowcs(unicode_name, name.c_str(), name_length);
	
	// Load the iamge from the exe as raw application data.
	HRSRC rsrcData = FindResource(NULL, unicode_name, RT_RCDATA);
	if (!rsrcData) {
		throw std::runtime_error("Failed to find resource.");
	}

	DWORD rsrcDataSize = SizeofResource(NULL, rsrcData);
	if (rsrcDataSize <= 0) {
		throw std::runtime_error("Size of resource is 0.");
	}

	HGLOBAL grsrcData = LoadResource(NULL, rsrcData);
	if (!grsrcData) {
		throw std::runtime_error("Failed to load resource.");
	}

	LPVOID firstByte = LockResource(grsrcData);
	if (!firstByte) {
		throw std::runtime_error("Failed to lock resource.");
	}

	sf::Image image;
	if (!image.loadFromMemory(firstByte, rsrcDataSize)) {
		throw std::runtime_error("Failed to load image from memory.");
	}

	return image;
}
