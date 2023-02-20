#include "Hooks.h"

#include "MinHook/MinHook.h"
#include <iostream>
#include <Windows.h>
#include <Psapi.h>

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
		}
		else {
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

#define FindSig(sig) FindSignatureModule("Growtopia.exe", sig);

std::string Hooks::ENetPeerSend_Sig{ "40 55 57 41 54 48 81 EC 90 00 00 00 48 8B E9 44 0F B6 E2 49 8B F8 41 8B C4" };
std::string Hooks::ENetPacketCreate_Sig{ "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 20 48 8B F1 41 8B E8 B9 30 00 00 00 48" };
std::string Hooks::ENetPacketDestroy_Sig{ "48 85 C9 74 34 53 48 83 EC 20 48 8B 41 20 48 8B D9 48 85 C0 74 02 FF D0 F6 43 08 04 75 0E 48 8B" };
std::string Hooks::HandleIncomingPacket_Sig{ "40 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 00 EE FF FF B8 00 13 00 00 E8" };

uintptr_t Hooks::HandleIncomingPacket_Addr   = 0;
uintptr_t Hooks::ENetPeerSend_Addr		     = 0;
uintptr_t Hooks::IsDebuggerPresent_Addr      = 0; 

HMODULE Hooks::GT_Module         = 0;
HMODULE Hooks::Kernel32_Module   = 0;
HMODULE Hooks::Iphlpapi_Module   = 0;

HANDLE Hooks::GT_Proc = 0;

void Hooks::Init() {

	MH_Initialize();

	Hooks::GT_Module = GetModuleHandle(NULL);
	Hooks::GT_Proc = GetCurrentProcess();
	Hooks::Kernel32_Module = LoadLibrary(L"Kernel32");
	Hooks::Iphlpapi_Module = LoadLibrary(L"Iphlpapi");

	Hooks::HandleIncomingPacket_Addr = FindSig(Hooks::HandleIncomingPacket_Sig.c_str());
	Hooks::ENetPeerSend_Addr = FindSig(Hooks::ENetPeerSend_Sig.c_str());
	Hooks::IsDebuggerPresent_Addr = (uintptr_t)GetProcAddress(Hooks::Kernel32_Module, "IsDebuggerPresent");
	Hooks::GetAdaptersAddresses_Addr = (uintptr_t)GetProcAddress(Hooks::Iphlpapi_Module, "GetAdaptersAddresses");

	std::cout << MH_CreateHook()

    //std::cout << "PacketHandlerHook status : " << MH_StatusToString(MH_CreateHook())

}

