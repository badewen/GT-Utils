#pragma once

#include <string>
#include "include/enet/enet.h"
#include <iphlpapi.h>

typedef void(__cdecl* ENetPacketDestroy_FuncSig)(ENetPacket* packet);
typedef int(__cdecl* ENetPeerSend_FuncSig)(ENetPeer* peer, enet_uint8 channelID, ENetPacket* packet);
//typedef ENetPacket* (__cdecl* ENetPeerReceive_FuncSig)(ENetPeer* peer, enet_uint8* channelID);
//typedef int(__cdecl* ENetHostService_FuncSig)(ENetHost* host, ENetEvent* event, enet_uint32 timeout);
//typedef void (*some_func_called_every_service_sig)(void* Block);
typedef void (*HandleIncomingPacket_FuncSig)(int64_t a1, ENetEvent* enet_event);
typedef bool(__stdcall* IsDebuggerPresent_FuncSig)();
typedef ENetPacket* (__cdecl* ENetPacketCreate_FuncSig)(const void* data, size_t data_length, enet_uint32 flags);

typedef ULONG (__stdcall* GetAdaptersAddresses_FuncSig)( ULONG Family, ULONG Flags, PVOID Reserved, PIP_ADAPTER_ADDRESSES AdapterAddresses, PULONG SizePointer);

class Hooks
{
public:
    static void Init();

public:
    static std::string ENetPeerSend_Sig      ;
    static std::string ENetPacketCreate_Sig  ;
    static std::string ENetPacketDestroy_Sig ;

    // this function is called every incoming packet, and also the one handling login.
    static std::string HandleIncomingPacket_Sig;

    // all commented sigs are kept for historical purposes

    // idk why, but this function just cant be used to hook received packets. 
    //std::string ENetPeerReceiveSig{ "48 89 74 24 10 57 48 83 EC 20 48 8D 81 40 01 00 00 48 8B F1 48 8B 08 48 8B FA 48 3B C8 75 0D 33 C0 48 8B 74 24 38 48 83 C4 20 5F C3 48 89 5C 24 30 E8 6A F2 FF FF 48 8B D8 48 85 FF 74 06 0F B6 48 15 88 0F 48 8B 78 58 48 FF 0F 48 8B 48 50 48 85 C9 74 05 E8 57 D3 FF FF 48 8B CB E8 4F D3 FF FF 48 8B 4F 18 48 8B C7 48 29 8E E0 01 00 00 48 8B 74 24 38 48 8B 5C 24 30 48 83 C4 20 5F C3 CC" };
   
    //std::string ENetHostServiceSig{ "48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 20 41 8B F0 48 8B FA 48 8B D9 48 85 D2 74 23 33 C0 89 02 48 89 42 08 48 89 42 18 E8 92 03 00 00 83 F8 FF 0F 84 83 01 00 00 83 F8 01 0F 84 8D 01 00 00 E8 CB 3A 00 00 89 43 58 03 F0 66 0F 1F 44 00 00 8B 53 58 8B CA 8B 43 30 2B C8 81 F9 00 5C 26 05 72 04 2B C2 EB 02 8B C1 3D E8 03 00 00 72 08 48 8B CB E8 A9 DC FF FF 41 B8 01 00 00 00 48 8B D7" };
    //std::string FuncCalledAfterHandlingPacketSig{ "48 85 C9 74 34 53 48 83 EC 20 48 8B 41 20 48 8B D9 48 85 C0 74 02 FF D0 F6 43 08 04 75 0E 48 8B 4B 10 48 85 C9 74 05 E8 64 DF FF FF 48 8B CB E8 5C DF FF FF" };
    
public:
    static uintptr_t HandleIncomingPacket_Addr;
    static uintptr_t ENetPeerSend_Addr;
    static uintptr_t IsDebuggerPresent_Addr;
    static uintptr_t GetAdaptersAddresses_Addr;

    static HandleIncomingPacket_FuncSig HandleIncomingPacket_Tramp;
    static ENetPeerSend_FuncSig ENetPeerSend_Tramp;
    static IsDebuggerPresent_FuncSig IsDebuggerPresent_Tramp;
    static GetAdaptersAddresses_FuncSig GetAdaptersAddresses_Tramp;

    static HMODULE GT_Module;
    static HMODULE Kernel32_Module;
    static HMODULE Iphlpapi_Module;

    static HANDLE GT_Proc;
};

