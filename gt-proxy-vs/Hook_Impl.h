#pragma once

#include <stdint.h>
#include <enet/enet.h>

extern void some_func_called_every_service_hook(void* Block);
extern int __cdecl enet_host_service_hook(ENetHost*, ENetEvent*, enet_uint32);
extern int __cdecl enet_peer_send_hook(ENetPeer* peer, enet_uint8 channelID, ENetPacket* packet);
//ENetPacket* __cdecl enet_peer_receive_hook(ENetPeer * peer, enet_uint8 * channelID);
extern void handle_incoming_packet_hook(int64_t a1, ENetEvent* enet_event);
extern bool __stdcall is_debugger_present_hook();
