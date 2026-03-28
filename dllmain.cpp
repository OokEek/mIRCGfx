// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "mIRCGfx.h"

namespace
{
	HANDLE getMutex() noexcept
	{
		static HANDLE hMutex{ nullptr };

		if (hMutex)
			return hMutex;

		TCHAR mutex[128]{};
		// add pid of mIRC.exe to name so mutex is specific to this instance of mIRC.
		// GetModuleHandle(nullptr) was returning a consistant result.
		_ts_snprintf(std::addressof(mutex[0]), std::size(mutex), TEXT("MIRCGFX_LOADED%lx"), GetCurrentProcessId()); // NB: calls user32.dll, is this ok? See warnings in DllMain() docs.
		// Enforce only one instance of dll loaded at a time.
		hMutex = CreateMutex(nullptr, TRUE, std::addressof(mutex[0]));

		if ((!hMutex) || (GetLastError() == ERROR_ALREADY_EXISTS))
			return nullptr;

		return hMutex;
	}
}
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
) noexcept
{
	UNREFERENCED_PARAMETER(lpReserved);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);

		if (!getMutex())
			return FALSE;

	}
	break;
	case DLL_THREAD_ATTACH:
		[[fallthrough]];
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
	{
		if (const HANDLE hMutex = getMutex(); hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}
	}
	break;
	default:
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void WINAPI LoadDll(LOADINFO* load)
{
	mIRCGfx::load(load);
}

extern "C" __declspec(dllexport) int WINAPI UnloadDll(int timeout)
{
	// DLL unloaded because mIRC exits or /dll -u used
	if (timeout != 1)
	{
		// free resources...
		mIRCGfx::unload();
		return 1;
	}
	// keep DLL in memory
	return 0;
}

extern "C" __declspec(dllexport) int WINAPI DllInfo(const HWND mWnd, const HWND aWnd, TCHAR* const data, const TCHAR* const parms, BOOL, BOOL)
{
	UNREFERENCED_PARAMETER(mWnd);
	UNREFERENCED_PARAMETER(aWnd);
	UNREFERENCED_PARAMETER(parms);

	_ts_strcpyn(data, _T("mIRCGfx DLL " DLL_VERSION " " DLL_STATE " By Ook ©2025 Compiled on " __DATE__ " " __TIME__), MIRC_BUFFER_SIZE_CCH);
	return 3;
}

