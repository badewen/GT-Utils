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
#include "./include/MinHook/MinHook.h"
#include <Psapi.h>
#include "./include/plog/Init.h"
#include "./include/plog/Log.h"
#include "./include/plog/Initializers/RollingFileInitializer.h"
#include <random>
#include <mutex>
#include <iphlpapi.h>



HINSTANCE dllh = 0;
std::mutex peer_mutex{};
ENetPeer* peer_ = 0;
enet_uint8 channelID = 0;

#define LOGI std::cout 

typedef void (__cdecl* enet_packet_destroy_sig)(ENetPacket* packet);
typedef int (__cdecl* enet_peer_send_sig)(ENetPeer * peer, enet_uint8 channelID, ENetPacket * packet);
typedef ENetPacket* (__cdecl* enet_peer_receive_sig)(ENetPeer * peer, enet_uint8 * channelID);
typedef int (__cdecl* enet_host_service_sig)(ENetHost * host, ENetEvent * event, enet_uint32 timeout);
typedef void (* some_func_called_every_service_sig)(void* Block);
typedef void (* handle_incoming_packet_sig)(int64_t a1, ENetEvent* enet_event);
typedef bool (__stdcall* is_debugger_present_sig)();
typedef ENetPacket* (__cdecl* enet_packet_create_sig)(const void* data, size_t data_length, enet_uint32 flags);

enet_packet_create_sig enet_packet_create;
is_debugger_present_sig is_debugger_present_tramp;
some_func_called_every_service_sig some_func_called_every_service_tramp;
enet_host_service_sig enet_host_service_tramp;
enet_peer_receive_sig enet_peer_receive_tramp;
enet_peer_send_sig peer_send_tramp;
handle_incoming_packet_sig handle_incoming_packet_tramp;
enet_packet_destroy_sig enet_packet_destroy;

//totally didnot copy and pasted from horion client!11!111

#define INRANGE(x, a, b) (x >= a && x <= b)
#define GET_BYTE(x) (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))
#define GET_BITS(x) (INRANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xa) : (INRANGE(x, '0', '9') ? x - '0' : 0))

uintptr_t FindSignatureModule(const char* szModule, const char* szSignature) {
	const char* pattern = szSignature;
	uintptr_t firstMatch = 0;
	static const auto rangeStart = (uintptr_t)GetModuleHandleA(szModule);
	static MODULEINFO miModInfo;
	static bool init = false;
	if (!init) {
		init = true;
		GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
	}
	static const uintptr_t rangeEnd = rangeStart + miModInfo.SizeOfImage;

	BYTE patByte = GET_BYTE(pattern);
	const char* oldPat = pattern;

	for (uintptr_t pCur = rangeStart; pCur < rangeEnd; pCur++) {
		if (!*pattern)
			return firstMatch;

		while (*(PBYTE)pattern == ' ')
			pattern++;

		if (!*pattern)
			return firstMatch;

		if (oldPat != pattern) {
			oldPat = pattern;
			if (*(PBYTE)pattern != '\?')
				patByte = GET_BYTE(pattern);
		}

		if (*(PBYTE)pattern == '\?' || *(BYTE*)pCur == patByte) {
			if (!firstMatch)
				firstMatch = pCur;

			if (!pattern[2] || !pattern[1])
				return firstMatch;

			//if (*(PWORD)pattern == '\?\?' || *(PBYTE)pattern != '\?')
			//pattern += 3;

			//else
			pattern += 2;
		} else {
			pattern = szSignature;
			firstMatch = 0;
		}
	}
	const char* sig = szSignature;  // Put sig in here to access it in debugger
	// This will not get optimized away because we are in debug
	// Leave this in here to quickly find bad signatures in case of updates
	printf("Signature dead: %s", szSignature);
	//if (false) {
	//	const char* msgToTheOverwhelmedDebugger = "SIGNATURE NOT FOUND";
	//	__debugbreak();
	//}

	return 0u;
}

void print_fuck_you(char byte_);
void some_func_called_every_service_hook(void* Block);
int __cdecl enet_host_service_hook(ENetHost *, ENetEvent *, enet_uint32);
int __cdecl enet_peer_send_hook(ENetPeer * peer, enet_uint8 channelID, ENetPacket * packet);
//ENetPacket* __cdecl enet_peer_receive_hook(ENetPeer * peer, enet_uint8 * channelID);
void handle_incoming_packet_hook(int64_t a1, ENetEvent* enet_event);
bool __stdcall is_debugger_present_hook();

// return false if not found.
bool modify_text_packet(std::string& text_packet, std::string data, std::string pakcet_name);
std::string random_number_int(size_t len, char min_ch = '0', char max_ch = '9');
std::string random_number_uint(size_t len, char min_ch = '0', char max_ch = '9');
std::string random_number_hex(int len);
std::string random_mac();

void mainhack();

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:    
            DisableThreadLibraryCalls(hinstDLL);
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainhack , nullptr, 0, 0);
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
    HMODULE gt_mod = 0;
    HANDLE gt_proc = 0;
    uintptr_t enet_peer_send_addr = 0;
    uintptr_t enet_peer_receive_addr = 0;
    uintptr_t enet_host_service_addr = 0;
    uintptr_t some_func_called_every_service_addr = 0;
    uintptr_t handle_incoming_packet_addr = 0;
    uintptr_t is_debugger_present_addr = 0;
    uint8_t some_packet[] = {0x4, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    freopen_s(&stdoutf, "packet_logs.txt", "w", stdout);
    std::cout << "HAHAHAHAHAH";
    
    MH_Initialize();

    gt_mod = GetModuleHandle(L"Growtopia.exe");
    gt_proc = GetCurrentProcess();

    enet_peer_send_addr = FindSignatureModule("Growtopia.exe", "40 55 57 41 54 48 81 EC 90 00 00 00 48 8B E9 44 0F B6 E2 49 8B F8 41 8B C4");
    //enet_peer_receive_addr = FindSignatureModule("Growtopia.exe", "48 89 74 24 10 57 48 83 EC 20 48 8D 81 40 01 00 00 48 8B F1 48 8B 08 48 8B FA 48 3B C8 75 0D 33 C0 48 8B 74 24 38 48 83 C4 20 5F C3 48 89 5C 24 30 E8 6A F2 FF FF 48 8B D8 48 85 FF 74 06 0F B6 48 15 88 0F 48 8B 78 58 48 FF 0F 48 8B 48 50 48 85 C9 74 05 E8 57 D3 FF FF 48 8B CB E8 4F D3 FF FF 48 8B 4F 18 48 8B C7 48 29 8E E0 01 00 00 48 8B 74 24 38 48 8B 5C 24 30 48 83 C4 20 5F C3 CC");
    enet_host_service_addr = FindSignatureModule("Growtopia.exe", "48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 20 41 8B F0 48 8B FA 48 8B D9 48 85 D2 74 23 33 C0 89 02 48 89 42 08 48 89 42 18 E8 92 03 00 00 83 F8 FF 0F 84 83 01 00 00 83 F8 01 0F 84 8D 01 00 00 E8 CB 3A 00 00 89 43 58 03 F0 66 0F 1F 44 00 00 8B 53 58 8B CA 8B 43 30 2B C8 81 F9 00 5C 26 05 72 04 2B C2 EB 02 8B C1 3D E8 03 00 00 72 08 48 8B CB E8 A9 DC FF FF 41 B8 01 00 00 00 48 8B D7");
    // some_func_called_every_service_addr = FindSignatureModule("Growtopia.exe", "48 85 C9 74 34 53 48 83 EC 20 48 8B 41 20 48 8B D9 48 85 C0 74 02 FF D0 F6 43 08 04 75 0E 48 8B 4B 10 48 85 C9 74 05 E8 64 DF FF FF 48 8B CB E8 5C DF FF FF");
    enet_packet_create = (enet_packet_create_sig)FindSignatureModule("Growtopia.exe", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 20 48 8B F1 41 8B E8 B9 30 00 00 00 48");
    enet_packet_destroy = (enet_packet_destroy_sig)FindSignatureModule("Growtopia.exe", "48 85 C9 74 34 53 48 83 EC 20 48 8B 41 20 48 8B D9 48 85 C0 74 02 FF D0 F6 43 08 04 75 0E 48 8B");

    // this is actually function to handle login, but it is always called every packet receive so why not.
    handle_incoming_packet_addr = FindSignatureModule("Growtopia.exe", "40 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 00 EE FF FF B8 00 13 00 00 E8");
    
    is_debugger_present_addr = (uintptr_t)GetProcAddress(LoadLibrary(L"Kernel32"), "IsDebuggerPresent");

    if (!is_debugger_present_addr) {
        std::cout << "is_debugger_present is null with code : " << GetLastError() << '\n';
    }

    // std::cout << MH_StatusToString(MH_CreateHook((void*)enet_peer_receive_addr, (LPVOID)enet_peer_receive_hook, (LPVOID*)&enet_peer_receive_tramp));
    std::cout << MH_StatusToString(MH_CreateHook((void*)enet_peer_send_addr, (LPVOID)enet_peer_send_hook, (LPVOID*)&peer_send_tramp));
    //std::cout << MH_StatusToString(MH_CreateHook((void*)enet_host_service_addr, (LPVOID)enet_host_service_hook, (LPVOID*)&enet_host_service_tramp));
    // std::cout << MH_StatusToString(MH_CreateHook((void*)some_func_called_every_service_addr, (LPVOID)some_func_called_every_service_hook, (LPVOID*)&some_func_called_every_service_tramp));
    std::cout << MH_StatusToString(MH_CreateHook((void*)handle_incoming_packet_addr, (LPVOID)handle_incoming_packet_hook, (LPVOID*)&handle_incoming_packet_tramp));
    std::cout << MH_StatusToString(MH_CreateHook((void*)is_debugger_present_addr, (LPVOID)is_debugger_present_hook, (LPVOID*)&is_debugger_present_tramp));
    MH_EnableHook(MH_ALL_HOOKS);

    std::cout << '\n' << random_number_uint(8) << '\n' << random_number_hex(8) << '\n' << random_mac() << '\n'; 

    if (!EnumProcessModulesEx(gt_proc, loaded_mod, sizeof(loaded_mod), &loaded_mod_count, LIST_MODULES_64BIT)) {
        std::cout << "EnumProcessModules Failed\n";
        std::cout << GetLastError() << '\n';
    }

    // 1023 + null terminator
    WCHAR mod_name[1024];

    std::cout << "LOADED MODULES : \n";

    for (int i = 0; i < loaded_mod_count/ sizeof(HMODULE); i++) {
        if (!GetModuleBaseNameW(gt_proc, loaded_mod[i], mod_name, sizeof(mod_name) - 1)) {
            std::cout << "GetModuleBaseName Failed";
            std::cout << GetLastError() << '\n';
        }
        std::cout << mod_name << '\n';
        memset(mod_name, 0, sizeof(mod_name));
    }


    while(1) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x0001) {
            break;
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x0001) {
            auto packet = enet_packet_create(some_packet, sizeof(some_packet), ENET_PACKET_FLAG_RELIABLE);
            peer_mutex.lock();
            enet_peer_send_hook(peer_, channelID, packet);
            peer_mutex.unlock();
            enet_packet_destroy(packet);
        }
    }

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    FreeConsole();
    fclose(stdoutf);
    FreeLibraryAndExitThread(dllh, 0);
}



int __cdecl enet_peer_send_hook(ENetPeer * peer, enet_uint8 channelID, ENetPacket * packet) {

    peer_ = peer;

    // lol, i think i read too many of ida decompiler code.
    if (*(uint32_t*)packet->data == 2) {
        char* buff = (char*)malloc(packet->dataLength - 4 + 1);

        memset(buff, 0, packet->dataLength - 4 + 1);
        memcpy(buff, packet->data + 4, packet->dataLength - 4);

        std::string bruh = { buff };
        free(buff);
        try {
            if (!modify_text_packet(bruh, "4.14", "game_version|")){
                goto skip;
            }

 //           //if (!modify_text_packet(bruh, random_number_uint(8), "token|")) {
 //           //    goto skip;
 //           //}

 //           //if (!modify_text_packet(bruh, random_number_hex(32), "rid|")) {
 //           //    goto skip;
 //           //}

 ///*           if (!modify_text_packet(bruh, random_number_hex(32), "UUIDToken|")) {
 //               goto skip;
 //           }*/

 //           if (!modify_text_packet(bruh, random_number_hex(32), "wk|")) {
 //               goto skip;
 //           }

 //           if (!modify_text_packet(bruh, random_mac(), "mac|")) {
 //               goto skip;
 //           }

 //           if (!modify_text_packet(bruh, random_number_uint(8), "user|")) {
 //               goto skip;
 //           }

        }
        catch (std::exception e) {
            std::cout << e.what();
        }
        std::cout << bruh << '\n';
        memcpy(packet->data + 4, bruh.c_str(), packet->dataLength - 4);
    }
skip:
    LOGI << "[PEER SEND]\n";
    LOGI << "DATA : \"";
    for(int i = 0; i < packet->dataLength; i++) {
       printf("0x%02x ", ((uint8_t*)(packet->data))[i]);
    }
    LOGI << "\"\n";
    
    LOGI << "in string : \"";

    for(int i = 0; i < packet->dataLength; i++) {
        print_fuck_you(*((uint8_t*)(((uintptr_t)packet->data) + i)));
    }
    LOGI << "\"\n\n";  

    return peer_send_tramp(peer, channelID, packet);
}

//ENetPacket* __cdecl enet_peer_receive_hook(ENetPeer * peer, enet_uint8 * channelID) {
//
//    ENetIncomingCommand* incomingCommand;
//    ENetPacket* packet;
//
//    incomingCommand = (ENetIncomingCommand*)enet_list_begin(&peer->dispatchedCommands);
//
//   packet = incomingCommand -> packet;
//
//    if (packet->dataLength > 100) {
//        goto some_LABEL;
//    }
//
//    LOGI << "[PEER RECEIVE]\n";
//    LOGI << "DATA : \"";
//    for(int i = 0; i < packet->dataLength; i++) {
//       printf("%#02x ",*((uint8_t*)(((uintptr_t)packet->data) + i)));
//    }
//    LOGI << "\n";
//
//    LOGI << "in string : \"";
//
//    for(int i = 0; i < packet->dataLength; i++) {
//        print_fuck_you(*((uint8_t*)(((uintptr_t)packet->data) + i)));
//    }
//    LOGI << "\"\n\n";
//
//some_LABEL:
//    return enet_peer_receive_tramp(peer, channelID);
//}

int __cdecl enet_host_service_hook (ENetHost * host, ENetEvent * event, enet_uint32 timeout) {

    if (host->peerCount <= 0 || host->connectedPeers <= 0)
        goto some_label;

    if (&(host->peers[0]) != peer_) {
        peer_mutex.lock();
        peer_ = &(host->peers[0]);
        peer_mutex.unlock();
        LOGI << "peer count : " << host->peerCount << "\nconnected peers : " << host->connectedPeers << '\n';
    }

    std::cout << "host service!\n";

some_label:

    return enet_host_service_tramp(host, event, timeout);
}

void some_func_called_every_service_hook(void* Block) {
    //register ENetEvent event asm("rax");

    //std::cout << "hehehahw \"" << event.packet->dataLength << '\n';
    return some_func_called_every_service_tramp(Block);

}

void handle_incoming_packet_hook(int64_t a1, ENetEvent* enet_event) {

    //if (enet_event->packet->dataLength > 400)
    //    goto skip;
    
    LOGI << "[INCOMING PACKET]\n";
    LOGI << "DATA : \"";


    for(int i = 0; i < enet_event->packet->dataLength; i++) {
        printf("0x%02x ", ((uint8_t*)enet_event->packet->data)[i]);
    }
    LOGI << "\n";

    LOGI << "in string : \"";

    for(int i = 0; i < enet_event->packet->dataLength; i++) {
        print_fuck_you(*((uint8_t*)(((uintptr_t)enet_event->packet->data) + i)));
    }
    LOGI << "\"\n\n";

skip:


    return handle_incoming_packet_tramp(a1, enet_event);
}

// completely override the function
bool __stdcall is_debugger_present_hook() {
    return false;
}

// same as random_number_uint but can go negative.
// returned size is len + 1 because negative sign.
std::string random_number_int(size_t len, char min_ch, char max_ch) {
    std::string min_str{};
    std::string max_str{};

    min_str.reserve(len);
    max_str.reserve(len);
    
    for (int i = 0; i < len; i++) {
        min_str.push_back(min_ch);
        max_str.push_back(max_ch);
    }

    min_str.replace(0, 1, "1");


    std::random_device rd;
    std::mt19937 gen {rd() };
    std::uniform_int_distribution<> distr{-(std::stoi(min_str)), std::stoi(max_str)};

    return std::to_string(distr(gen));
}

std::string random_number_uint(size_t len, char min_ch, char max_ch) {
    
    std::string min_str{};
    std::string max_str{};

    min_str.reserve(len);
    max_str.reserve(len);
    
    for (int i = 0; i < len; i++) {
        min_str.push_back(min_ch);
        max_str.push_back(max_ch);
    }

    min_str.replace(0, 1, "1");

    std::random_device rd;
    std::mt19937 gen {rd() };
    std::uniform_int_distribution<> distr{std::stoi(min_str), std::stoi(max_str)};

    return std::to_string(distr(gen));
}

std::string random_number_hex(int len) {
    std::stringstream str_stream {};

    for (int i = 0; i < len; i++) {
        str_stream << std::hex << (std::stoi(random_number_uint(1, '0', '8')) + std::stoi(random_number_uint(1, '0', '8'))); 
    }

    return str_stream.str();

}

std::string random_mac() {

    std::string mac {};
    // 17
    mac.reserve(17);
    std::string gen_num  = random_number_hex(2);

    for (int i = 0; i < 6; i++) {
        gen_num = random_number_hex(2);
        if (gen_num.length() > 2) { 
            i--;
            continue;
        }
        mac.append(gen_num);
        if (i == 5) 
            break;
        mac.push_back(':');
    }
    return mac;
}

char allowed_chars[] = {' ','a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 0xa, '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '[', ']', '\\', ';', '\'', ',', '.', '/', '{', '}', '|', ':', '"', '<', '>', '?'};

void print_fuck_you(char byte_) {
    for (int i = 0; i < sizeof(allowed_chars); i++) {
        if (allowed_chars[i] == byte_) {
            LOGI << byte_;
            return;
        }
    }
    return;
}

bool modify_text_packet(std::string& text_packet, std::string data, std::string packet_name) {
    if (data.length() == 0) return false;
    size_t packet_name_pos = text_packet.find(packet_name);
    
    if (packet_name_pos == std::string::npos) { return false; }

    size_t text_packet_end_pos = text_packet.find("\n", packet_name_pos);

    if (text_packet_end_pos == std::string::npos) {
        text_packet_end_pos = text_packet.length();
    }

    size_t packet_data_field_length = text_packet_end_pos - packet_name_pos - packet_name.length();

    size_t writeable_length = std::clamp(data.length(), (size_t)1, packet_data_field_length);

    text_packet.replace(packet_name_pos + packet_name.length(), writeable_length, data);

    return true;

}
