#pragma once

#include <array>
#include <cstdarg>
#include <iostream>
#include <Windows.h>

#include "minhook/MinHook.h"

enum LogLevel : int
{
	LOG_INFO,
	LOG_ERROR
};

class Astral
{
private:
	std::string path = "C:\\AstraL\\DLL\\cheat.dll";
	std::array<LPCSTR, 3> dirs = { "C:\\AstraL", "C:\\AstraL\\DLL", "C:\\AstraL\\DLL\\Configs" };

	DWORD size = 0;
	LPVOID buffer = nullptr;
	std::uintptr_t memory = 0;

	std::uintptr_t authCheck = 0x141C57;
	std::uintptr_t openMenu = 0x1CF1DD8;
	std::array<uint8_t, 5> authPatch = { 0xE9, 0x0C, 0x02, 0x00, 0x00 };

	void Log(LogLevel logLevel, LPCSTR log, ...);

public:
	void CreateConsole();

	void UnpackResource(HINSTANCE instance);

	void WriteResrouce();

	void SetupHooks();

	void LoadCheat();

	void PatchAuthorization();

	void OpenBrowser();
};

extern Astral* astral;
