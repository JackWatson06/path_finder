/*
* This file implements several different functions that handle loading options from an init file into the application.
* Upon ending the simulation we will save the current configuration to the file system.
*/
#include "headers/Application.h"

using namespace Simplex;

void Application::ReadConfig(void)
{
	String sRoot = system->m_pFolder->GetFolderRoot();
	String App = system->GetAppName();
	App = sRoot + App + ".ini";

	FileReader reader;

	if (reader.ReadFile(App.c_str()) == OUT_ERR_FILE_MISSING) {
		return;
	}

	reader.Rewind();
	while (reader.ReadNextLine() == OUT_RUNNING) {
		String sWord = reader.GetFirstWord();

		int nLenght = reader.m_sLine.length();
		char* zsTemp = new char[nLenght];

		if (sWord == "Fullscreen:") {
			int nValue;
			sscanf_s(reader.m_sLine.c_str(), "Fullscreen: %d", &nValue);
			system->SetWindowFullscreen(nValue != 0);
		} else if (sWord == "Borderless:") {
			int nValue;
			sscanf_s(reader.m_sLine.c_str(), "Borderless: %d", &nValue);
			system->SetWindowBorderless(nValue != 0);
		} else if (sWord == "FrameRate:") {
			int nValue;
			sscanf_s(reader.m_sLine.c_str(), "FrameRate: %d", &nValue);
			system->SetMaxFrameRate(static_cast<uint>(nValue));
		} else if (sWord == "Resolution:") {
			int nValue1;
			int nValue2;
			sscanf_s(reader.m_sLine.c_str(), "Resolution: [ %d x %d ]", &nValue1, &nValue2);
			system->SetWindowWidth(nValue1);
			system->SetWindowHeight(nValue2);
		} else if (sWord == "Ambient:") {
			float fValueX;
			float fValueY;
			float fValueZ;
			sscanf_s(reader.m_sLine.c_str(), "Ambient: [%f,%f,%f]", &fValueX, &fValueY, &fValueZ);
			light_manager->SetColor(vector3(fValueX, fValueY, fValueZ), 0);
		} else if (sWord == "Background:") {
			float fValueX;
			float fValueY;
			float fValueZ;
			float fValueW;
			sscanf_s(reader.m_sLine.c_str(), "Background: [%f,%f,%f,%f]", &fValueX, &fValueY, &fValueZ, &fValueW);
			background_color = vector4(fValueX, fValueY, fValueZ, fValueW);
		} else if (sWord == "AmbientPower:") {
			float fValue;
			sscanf_s(reader.m_sLine.c_str(), "AmbientPower: %f", &fValue);
			light_manager->SetIntensity(fValue, 0);
		} else if (sWord == "Data:") {
			sscanf_s(reader.m_sLine.c_str(), "Data: %s", zsTemp, nLenght);
			system->m_pFolder->SetFolderData(zsTemp);
		} else if (sWord == "BTO:") {
			sscanf_s(reader.m_sLine.c_str(), "BTO: %s", zsTemp, nLenght);
			system->m_pFolder->SetFolderMBTO(zsTemp);
		} else if (sWord == "FBX:") {
			sscanf_s(reader.m_sLine.c_str(), "FBX: %s", zsTemp, nLenght);
			system->m_pFolder->SetFolderMFBX(zsTemp);
		} else if (sWord == "OBJ:") {
			sscanf_s(reader.m_sLine.c_str(), "OBJ: %s", zsTemp, nLenght);
			system->m_pFolder->SetFolderMOBJ(zsTemp);
		} else if (sWord == "Level:") {
			sscanf_s(reader.m_sLine.c_str(), "Level: %s", zsTemp, nLenght);
			system->m_pFolder->SetFolderLVL(zsTemp);
		} else if (sWord == "Textures:") {
			sscanf_s(reader.m_sLine.c_str(), "Textures: %s", zsTemp, nLenght);
			system->m_pFolder->SetFolderTextures(zsTemp);
		}

		delete[] zsTemp;
		zsTemp = nullptr;
	}
	reader.CloseFile();
}

void Application::WriteConfig(void)
{
	// Write the configuration for this application
	String sRoot = system->m_pFolder->GetFolderRoot();
	String App = system->GetAppName();
	App = sRoot + App + ".ini";

	FILE* pFile;
	fopen_s(&pFile, App.c_str(), "w");
	if (!pFile) {
		return;
	}

	rewind(pFile);
	fprintf(pFile, "# Configuration file for the program: %s", system->GetAppName().c_str());

	fprintf(pFile, "\n\nFullscreen: ");
	if (system->IsWindowFullscreen()) {
		fprintf(pFile, "1");
	} else  {
		fprintf(pFile, "0");
	}

	fprintf(pFile, "\nBorderless: ");
	if (system->IsWindowBorderless()) {
		fprintf(pFile, "1");
	} else {
		fprintf(pFile, "0");
	}

	fprintf(pFile, "\n\nFrameRate: ");
	fprintf(pFile, "%d", system->GetMaxFrameRate());

	fprintf(pFile, "\n\nResolution: [ %d x %d ]", system->GetWindowWidth(), system->GetWindowHeight());
	fprintf(pFile, "\n# Resolution: [ 640 x 480 ]");
	fprintf(pFile, "\n# Resolution: [ 1280 x 720 ]");
	fprintf(pFile, "\n# Resolution: [ 1680 x 1050 ]");
	fprintf(pFile, "\n# Resolution: [ 1920 x 1080 ]");
	fprintf(pFile, "\n# Resolution: [ 2560 x 1080 ]");


	fprintf(pFile, "\n\nAmbient: [%.2f,%.2f,%.2f]",
		light_manager->GetColor(0).r, light_manager->GetColor(0).g, light_manager->GetColor(0).b);
	fprintf(pFile, "\nAmbientPower: %.2f", light_manager->GetIntensity(0));

	fprintf(pFile, "\n\nBackground: [%.3f,%.3f,%.3f,%.3f]",
		background_color.r, background_color.g, background_color.b, background_color.a);

	fprintf(pFile, "\n\n# Folders:");
	fprintf(pFile, "\nData:		%s", system->m_pFolder->GetFolderData().c_str());
	fprintf(pFile, "\nBTO:		%s", system->m_pFolder->GetFolderMBTO().c_str());
	fprintf(pFile, "\nFBX:		%s", system->m_pFolder->GetFolderMFBX().c_str());
	fprintf(pFile, "\nOBJ:		%s", system->m_pFolder->GetFolderMOBJ().c_str());
	fprintf(pFile, "\nLevel:	%s", system->m_pFolder->GetFolderLVL().c_str());
	fprintf(pFile, "\nTextures:	%s", system->m_pFolder->GetFolderTextures().c_str());

	fclose(pFile);
}
