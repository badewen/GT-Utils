#pragma once

#include <stdint.h>
#include <enet/enet.h>
#include <iphlpapi.h>
#include <mutex>
#include "ImGuI/imgui.h"
#include <d3d9.h>

extern bool IsImguiInit;
extern HWND GT_HWND;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//extern void some_func_called_every_service_hook(void* Block);
//extern int __cdecl enet_host_service_hook(ENetHost*, ENetEvent*, enet_uint32);
//ENetPacket* __cdecl enet_peer_receive_hook(ENetPeer * peer, enet_uint8 * channelID);
extern int __cdecl ENetPeerSend_Hook(ENetPeer* peer, enet_uint8 channelID, ENetPacket* packet);
extern void HandleIncomingPacket_Hook(int64_t a1, ENetEvent* enet_event);
extern bool __stdcall IsDebuggerPresent_Hook();
extern ULONG __stdcall GetAdaptersAddresses_Hook(ULONG Family, ULONG Flags, PVOID Reserved, PIP_ADAPTER_ADDRESSES AdapterAddresses, PULONG SizePointer);
extern HRESULT __stdcall D3D9_EndScene_Hook(IDirect3DDevice9Ex* this_);

extern LRESULT __stdcall WndProc_Hook(HWND hwnd, UINT uMsg, WPARAM Wparam, LPARAM lParam);


