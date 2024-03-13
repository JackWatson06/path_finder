/*
* This file implements methods found in the FileSyste.h header which pertains to interactions with the systems
* underlying file system.
*/
#include "headers/FileSystem.h"
#include <locale>
#include <codecvt>

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

Simplex::FileSystem::FileSystem() { Init(); }

void Simplex::FileSystem::Init() 
{
	//https://learn.microsoft.com/en-us/windows/win32/services/installing-a-service
	TCHAR exe_path[MAX_PATH];
	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fname[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];

	if (!GetModuleFileName(NULL, exe_path, MAX_PATH)) {
		printf("Cannot install service (%d)\n", GetLastError());
		return;
	}

	_wsplitpath_s(exe_path, drive, dir, fname, ext);

	//https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	Folder::GetInstance()->SetFolderRoot(converter.to_bytes(dir));
	Folder::GetInstance()->SetFolderMOBJ("Objects\\");
}

sf::Image FileSystem::LoadImageFromResource(const std::string& name)
{	
	std::wstring stemp = std::wstring(name.begin(), name.end());

	// Load the iamge from the exe as raw application data.
	HRSRC rsrcData = FindResource(NULL, stemp.c_str(), RT_RCDATA);

	if (rsrcData == nullptr) {
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
