#pragma once

struct SWH_DATA {
	DWORD unk_0;				
	DWORD unk_4;				
	DWORD unk_8;				
	DWORD unk_c;			
	DWORD unk_10;		
	DWORD unk_14;	
	UNICODE_STRING lpDllPath;
	DWORD unk_20;
};

#define LDR_DLL_NOTIFICATION_REASON_LOADED   1
#define LDR_DLL_NOTIFICATION_REASON_UNLOADED 2

#define ThreadQuerySetWin32StartAddress 9

#define __ClientLoadLibrary_idx 74


typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA {
	ULONG Flags;                    //Reserved.
	PCUNICODE_STRING FullDllName;   //The full path name of the DLL module.
	PCUNICODE_STRING BaseDllName;   //The base file name of the DLL module.
	PVOID DllBase;                  //A pointer to the base address for the DLL in memory.
	ULONG SizeOfImage;              //The size of the DLL image, in bytes.
} LDR_DLL_LOADED_NOTIFICATION_DATA, *PLDR_DLL_LOADED_NOTIFICATION_DATA;

typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA {
	ULONG Flags;                    //Reserved.
	PCUNICODE_STRING FullDllName;   //The full path name of the DLL module.
	PCUNICODE_STRING BaseDllName;   //The base file name of the DLL module.
	PVOID DllBase;                  //A pointer to the base address for the DLL in memory.
	ULONG SizeOfImage;              //The size of the DLL image, in bytes.
} LDR_DLL_UNLOADED_NOTIFICATION_DATA, *PLDR_DLL_UNLOADED_NOTIFICATION_DATA;


typedef union _LDR_DLL_NOTIFICATION_DATA {
	LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
	LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
} LDR_DLL_NOTIFICATION_DATA, *PLDR_DLL_NOTIFICATION_DATA;

typedef VOID (CALLBACK* PLDR_DLL_NOTIFICATION_FUNCTION)(ULONG NotificationReason,
	_In_     PLDR_DLL_NOTIFICATION_DATA NotificationData,
	_In_opt_ PVOID Context
);

typedef int (WINAPI * _ClientLoadLibrary)(SWH_DATA * data);

typedef NTSTATUS (WINAPI* _LdrRegisterDllNotification)(ULONG Flags,PLDR_DLL_NOTIFICATION_FUNCTION NotificationFunction,PVOID Context,PVOID *Cookie);
typedef NTSTATUS (WINAPI* _LdrUnregisterDllNotification)(PVOID Cookie);


_ClientLoadLibrary o_ClientLoadLibrary = 0;


LPVOID * Get_apfnDispatch() {
#ifdef _M_IX86 
	return (LPVOID *)*(DWORD *)(__readfsdword(0x30) + 0x2C);
#else
	return (LPVOID *)*(DWORD64 *)(__readgsqword(0x60) + 0x58);
#endif
}


int WINAPI h_ClientLoadLibrary(SWH_DATA * data) {

	wprintf(L"window hook load dll %s\n", data->lpDllPath.Buffer);

	if (!anti_cheat->check_dll_on_legit(data->lpDllPath.Buffer)) {
		return 0;
	}

	return o_ClientLoadLibrary(data);
}


VOID WINAPI dll_notify(ULONG NotificationReason,
	_In_     PLDR_DLL_NOTIFICATION_DATA NotificationData,
	_In_opt_ PVOID Context
) {

	switch (NotificationReason) {
		case LDR_DLL_NOTIFICATION_REASON_LOADED: {
			anti_cheat->on_load_dll((HMODULE)NotificationData->Loaded.DllBase, NotificationData->Loaded.FullDllName->Buffer);
			break;
		}

		case LDR_DLL_NOTIFICATION_REASON_UNLOADED: {

			break;
		}
	}
}

bool c_anti_hack::check_dll_on_legit(std::wstring path) {
	int filename_idx = 0;
	for (filename_idx = path.length(); filename_idx > 0; filename_idx--) { if (path[filename_idx] == '\\') { filename_idx++;  break; } }

	if (!lstrcmpW(&path[filename_idx], L"TestDll.dll")) {
		return false;
	}

	return true;
}


void c_anti_hack::on_load_dll(HMODULE hmod, std::wstring path) {
	if (check_dll_on_legit(path) && !check_current_thread()) {
		wprintf(L"passed load dll [%s]\n", path.c_str());
		return;
	}
	else {
		wprintf(L"closed load dll [%s]\n", path.c_str());
		TerminateThread(GetCurrentThread(), 1);
	}
}

bool c_anti_hack::init_dll_profiler() {

	//dll load callback
	//DllNotification works only on Vista or later
	_LdrRegisterDllNotification LdrRegisterDllNotification = (_LdrRegisterDllNotification)GetProcAddress(GetModuleHandle("ntdll.dll"), "LdrRegisterDllNotification");
	
	if (LdrRegisterDllNotification) {
		LdrRegisterDllNotification(0, dll_notify, 0, &this->dll_notify_cookie);
	}

	//setwindowshook load dll
	DWORD oldprot;
	LPVOID * apfnDispatch = Get_apfnDispatch();
	if (apfnDispatch) {
		o_ClientLoadLibrary = (_ClientLoadLibrary)apfnDispatch[__ClientLoadLibrary_idx];

		VirtualProtect(apfnDispatch, 117 * sizeof(LPVOID), PAGE_EXECUTE_READWRITE, &oldprot);
		apfnDispatch[__ClientLoadLibrary_idx] = h_ClientLoadLibrary;
		VirtualProtect(apfnDispatch, 117 * sizeof(LPVOID), oldprot, &oldprot);

		return true;
	}
	else {
		return false;
	}
}


void c_anti_hack::uninit_dll_profiler() {

	_LdrUnregisterDllNotification LdrUnregisterDllNotification = (_LdrUnregisterDllNotification)GetProcAddress(GetModuleHandle("ntdll.dll"), "LdrUnregisterDllNotification");

	if (LdrUnregisterDllNotification) {
		LdrUnregisterDllNotification(this->dll_notify_cookie);
	}

	DWORD oldprot;
	LPVOID * apfnDispatch = Get_apfnDispatch();
	if (apfnDispatch && o_ClientLoadLibrary) {
		VirtualProtect(apfnDispatch, 117 * sizeof(LPVOID), PAGE_EXECUTE_READWRITE, &oldprot);
		apfnDispatch[__ClientLoadLibrary_idx] = o_ClientLoadLibrary;
		VirtualProtect(apfnDispatch, 117 * sizeof(LPVOID), oldprot, &oldprot);
	}
}