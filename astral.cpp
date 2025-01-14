#include "astral.h"

Astral* astral = new Astral();

decltype(&RegOpenKeyExA) oRegOpenKeyExA;
static LSTATUS hkRegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
	if (lstrcmpA(lpSubKey, "SOFTWARE\\AstraL") == 0)
	{
		astral->PatchAuthorization();
	}

	return oRegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

void Astral::Log(LogLevel logLevel, LPCSTR log, ...)
{
	va_list args;
	va_start(args, log);

	switch (logLevel)
	{
	case LOG_ERROR:
		fputs("[ERROR] ", stdout);
		break;

	case LOG_INFO:
		fputs("[INFO] ", stdout);
		break;
	}

	vprintf(log, args);
	fputs("\n", stdout);

	va_end(args);
}

void Astral::CreateConsole()
{
	if (!AllocConsole())
	{
		MessageBoxA(NULL, "Failed to allocate console", "Error", MB_ICONERROR);
		ExitProcess(0);
	}

	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	SetConsoleTitleA("by primordial.dev | 6635 uid");
}

void Astral::UnpackResource(HINSTANCE instance)
{
	Log(LOG_INFO, "Unpacking resource...");
	if (!instance)
	{
		Log(LOG_ERROR, "UR: 1");
		return;
	}

	HRSRC offset = FindResourceA(instance, (LPCSTR)101, "CHEAT");
	if (!offset)
	{
		Log(LOG_ERROR, "UR: 2");
		return;
	}

	HGLOBAL load = LoadResource(instance, offset);
	if (!load)
	{
		Log(LOG_ERROR, "UR: 3");
		return;
	}

	size = SizeofResource(instance, offset);
	if (!size)
	{
		Log(LOG_ERROR, "UR: 4");
		return;
	}

	buffer = LockResource(load);
	if (!buffer)
	{
		Log(LOG_ERROR, "UR: 5");
		return;
	}

	Log(LOG_INFO, "UR Buffer: %p", buffer);
	Log(LOG_INFO, "UR Size: %lu", size);
	Log(LOG_INFO, "Unpacked!");
}

void Astral::WriteResrouce()
{
	Log(LOG_INFO, "Writing resrouce...");

	for (LPCSTR dir : dirs)
	{
		CreateDirectoryA(dir, NULL);
	}

	HANDLE file = CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
	{
		Log(LOG_ERROR, "WR: 1");
		return;
	}

	DWORD bytes_written;
	if (!WriteFile(file, buffer, size, &bytes_written, NULL))
	{
		Log(LOG_ERROR, "WR: 2");
		return;
	}

	if (bytes_written != size)
	{
		Log(LOG_ERROR, "WR: 3");
		return;
	}

	if (!CloseHandle(file))
	{
		Log(LOG_ERROR, "WR: 4");
		return;
	}

	Log(LOG_INFO, "Writed!");
}

void Astral::SetupHooks()
{
	Log(LOG_INFO, "Setup hooks...");

	if (MH_Initialize() != MH_OK)
	{
		Log(LOG_ERROR, "SH: 1");
		return;
	}

	if (MH_CreateHook(&RegOpenKeyExA, hkRegOpenKeyExA, (LPVOID*)&oRegOpenKeyExA) != MH_OK)
	{
		Log(LOG_ERROR, "SH: 2");
		return;
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
	{
		Log(LOG_ERROR, "SH: 3");
		return;
	}

	Log(LOG_INFO, "Hooks created!");
}

void Astral::LoadCheat()
{
	Log(LOG_INFO, "Loading cheat...");
	memory = std::uintptr_t(LoadLibraryA(path.c_str()));

	if (!memory)
	{
		Log(LOG_ERROR, "LC: 1");
	}
}

void Astral::PatchAuthorization()
{
	Log(LOG_INFO, "Patching authorization...");

	DWORD protection = 0;
	LPVOID address = LPVOID(memory + authCheck);
	
	VirtualProtect(address, authPatch.size(), PAGE_EXECUTE_READWRITE, &protection);
	memcpy(address, authPatch.data(), authPatch.size());
	Log(LOG_INFO, "Patched!");

	*(bool*)(memory + openMenu) = true;
	Log(LOG_INFO, "Pwned!");
}

void Astral::OpenBrowser()
{
	ShellExecuteA(nullptr, "open", "https://t.me/trinityresell", nullptr, nullptr, SW_SHOWNORMAL);
}

static void CrackThread(HINSTANCE instance)
{
	astral->CreateConsole();

	astral->UnpackResource(instance);
	astral->WriteResrouce();

	astral->SetupHooks();
	astral->LoadCheat();

	astral->OpenBrowser();
}

static BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
		CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(CrackThread), instance, 0, NULL);
    }

    return TRUE;
}