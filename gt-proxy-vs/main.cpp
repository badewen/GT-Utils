#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "./include/enet/enet.h"
#include <Windows.h>
#include "main.h"
#include <ctime>
#include <exception>
#include <ios>
#include <sstream>
#include <string.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <string_view>
#include "./include/MinHook/include/MinHook.h"
#include <Psapi.h>
#include <random>
#include <mutex>
#include <iphlpapi.h>

#include "Hooks_Setup.h"
#include "Utils.h"
#include "ImGuI/imgui.h"
#include "ImGuI/imgui_impl_dx9.h"
#include "ImGuI/imgui_impl_win32.h"
#include "PacketLogBuf.h"

#include "Gui.h"
#include "Hook_Virt.h"
#include "Hook_Impl.h"

#include "SecureEngineSDK64_FuncExports.h"

HINSTANCE dllh = 0;

bool IsImguiInit = false;
HWND GT_HWND = 0;

#define LOGI std::cout

void mainhack();

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDLL);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainhack, nullptr, 0, 0);
        dllh = hinstDLL;
        break;

    case DLL_THREAD_ATTACH:

    case DLL_THREAD_DETACH:

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

void mainhack() {
    AllocConsole();

    HMODULE loaded_mod[1024];
    DWORD loaded_mod_count = 0;
    FILE* stdoutf = 0;
    HANDLE gt_proc = 0;

    uint8_t some_packet[] = { 0x4, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    //freopen_s(&stdoutf, "packet_logs.txt", "w", stdout);
    freopen_s(&stdoutf, "CONOUT$", "w", stdout);

    gt_proc = GetCurrentProcess();
    GT_HWND = Utils::get_hwnd();

    PacketLogBuf::Init();
    Hooks::Init();

    if (!GT_HWND) {
        std::cout << "wtf hwnd null\n";
    }
    std::cout << '\n' << Utils::random_number(8, "0", "99999999") << '\n' << Utils::random_number_hex(8) << '\n' << Utils::random_mac() << '\n' << Utils::random_ip() << '\n';

    if (!EnumProcessModulesEx(gt_proc, loaded_mod, sizeof(loaded_mod), &loaded_mod_count, LIST_MODULES_64BIT)) {
        std::cout << "EnumProcessModules Failed\n";
        std::cout << GetLastError() << '\n';
    }

    // 1023 + null terminator
    WCHAR mod_name[2048];

    std::cout << "LOADED MODULES : \n";

    for (int i = 0; i < loaded_mod_count / sizeof(HMODULE); i++) {
        if (!GetModuleBaseNameW(gt_proc, loaded_mod[i], mod_name, sizeof(mod_name) - 1)) {
            std::cout << "GetModuleBaseName Failed";
            std::cout << GetLastError() << '\n';
        }
        std::wcout << mod_name << '\n';
        memset(mod_name, 0, sizeof(mod_name));
    }

    Timer tm{};
    while (1) {
        if (GetAsyncKeyState(VK_OEM_MINUS)) {
            break;
        }

        if (Gui::ChangeMac) {
            Generated_Mac = Utils::random_mac();
            Gui::ChangeMac = false;
        }

        if (Gui::ChangeIp) {
            Generated_Ip = Utils::random_ip();
            Gui::ChangeIp = false;
        }

        if (Gui::ChangeUserId) {
            Generated_UserId = Utils::random_number(8, "11111111", "99999999");
            Gui::ChangeUserId = false;
        }

        PacketLogBuf::ProcessQueue();
        PacketLogBuf::Apply();

    }

    Hook::DisableAllHook();
    Hook::UnInit();

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    SetWindowLongPtr(GT_HWND, GWLP_WNDPROC, (LONG_PTR)Hooks::WndProc_O);

    FreeConsole();
    fclose(stdoutf);
    FreeLibraryAndExitThread(dllh, 0);
}