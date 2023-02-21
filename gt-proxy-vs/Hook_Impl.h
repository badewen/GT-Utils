#pragma once

#include <stdint.h>
#include <enet/enet.h>
#include <iphlpapi.h>

//extern void some_func_called_every_service_hook(void* Block);
//extern int __cdecl enet_host_service_hook(ENetHost*, ENetEvent*, enet_uint32);
//ENetPacket* __cdecl enet_peer_receive_hook(ENetPeer * peer, enet_uint8 * channelID);
extern int __cdecl ENetPeerSend_Hook(ENetPeer* peer, enet_uint8 channelID, ENetPacket* packet);
extern void HandleIncomingPacket_Hook(int64_t a1, ENetEvent* enet_event);
extern bool __stdcall IsDebuggerPresent_Hook();
extern ULONG GetAdaptersAddresses_Hook(ULONG Family, ULONG Flags, PVOID Reserved, PIP_ADAPTER_ADDRESSES AdapterAddresses, PULONG SizePointer);

