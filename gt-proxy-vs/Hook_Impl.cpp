#include "Hook_Impl.h"

#include <iostream>
#include "Hooks.h"
#include "Utils.h"

#include "PacketLogBuf.h"
#include "ImGuI/imgui_impl_dx9.h"
#include "ImGuI/imgui_impl_win32.h"
#include "ImGuI/imgui.h"
#include "Gui.h"
#include <winsock.h>

#define LOGI std::cout

std::string Generated_Ip{};
std::string Generated_Mac{};
std::string Generated_UserId{};

int __cdecl ENetPeerSend_Hook(ENetPeer* peer, enet_uint8 channelID, ENetPacket* packet) {

    // lol, i think i read too many of ida decompiler code.
    if (*(uint32_t*)packet->data == 2) {
        char* buff = (char*)malloc(packet->dataLength - 4 + 1);

        memset(buff, 0, packet->dataLength - 4 + 1);
        memcpy(buff, packet->data + 4, packet->dataLength - 4);

        std::string bruh = { buff };
        free(buff);

        std::cout << "HUH??";

        try {
            if (!Utils::modify_text_packet(bruh, std::to_string(Gui::SpoofedVersion), "game_version|")) {
                std::cout << "wtf";
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
            if (Gui::SpoofMac) {
                if (!Utils::modify_text_packet(bruh, Generated_Mac, "mac|")) {
                    goto skip;
                }
            }

            if (Gui::SpoofUserId) {
                if (!Utils::modify_text_packet(bruh, Generated_UserId, "user|")) {
                    goto skip;
                }
            }

        }
        catch (std::exception e) {
            std::cout << e.what();
        }
        std::cout << bruh << '\n';
        memcpy(packet->data + 4, bruh.c_str(), packet->dataLength - 4);
    }
skip:
    PacketLogBuf::AppedToQueue(packet->data, packet->dataLength, PacketReportTypeEnum_Sent);

    return Hooks::ENetPeerSend_Tramp(peer, channelID, packet);
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

//int __cdecl enet_host_service_hook(ENetHost* host, ENetEvent* event, enet_uint32 timeout) {
//
//    if (host->peerCount <= 0 || host->connectedPeers <= 0)
//        goto some_label;
//
//    if (&(host->peers[0]) != peer_) {
//        peer_mutex.lock();
//        peer_ = &(host->peers[0]);
//        peer_mutex.unlock();
//        LOGI << "peer count : " << host->peerCount << "\nconnected peers : " << host->connectedPeers << '\n';
//    }
//
//    std::cout << "host service!\n";
//
//some_label:
//
//    return enet_host_service_tramp(host, event, timeout);
//}

//void some_func_called_every_service_hook(void* Block) {
//    //register ENetEvent event asm("rax");
//
//    //std::cout << "hehehahw \"" << event.packet->dataLength << '\n';
//    return some_func_called_every_service_tramp(Block);
//
//}

void HandleIncomingPacket_Hook(int64_t a1, ENetEvent* enet_event) {

    //if (enet_event->packet->dataLength > 400)
    //    goto skip;
    
    PacketLogBuf::AppedToQueue(enet_event->packet->data, enet_event->packet->dataLength, PacketReportTypeEnum_Received);

skip:
    return Hooks::HandleIncomingPacket_Tramp(a1, enet_event);
}

// completely override the function
bool __stdcall IsDebuggerPresent_Hook() {
    return false;
}

ULONG __stdcall GetAdaptersAddresses_Hook(ULONG Family, ULONG Flags, PVOID Reserved, PIP_ADAPTER_ADDRESSES AdapterAddresses, PULONG SizePointer) {

    ULONG ret_val = Hooks::GetAdaptersAddresses_Tramp(Family, Flags, Reserved, AdapterAddresses, SizePointer);

    //if (!Gui::SpoofIp && !Gui::SpoofMac)
    //    goto label;

    if (Gui::SpoofIp)
        ((sockaddr_in*)AdapterAddresses->FirstUnicastAddress->Address.lpSockaddr)->sin_addr.S_un.S_addr = inet_addr(Generated_Ip.c_str());
    
    //if (Gui::SpoofMac) {
    //    for (int i = 0; i < AdapterAddresses->PhysicalAddressLength; i++) {
    //        AdapterAddresses->PhysicalAddress[i] = bruh[i];
    //    }
    //}


label:
    return ret_val;
}

HRESULT __stdcall D3D9_EndScene_Hook(IDirect3DDevice9Ex* this_) {
    if (!IsImguiInit) {

        D3DDEVICE_CREATION_PARAMETERS prm{};

        this_->GetCreationParameters(&prm);

        ImGui::CreateContext();
        ImGui_ImplDX9_Init(this_);
        ImGui_ImplWin32_Init(prm.hFocusWindow);
        ImGui_ImplWin32_EnableDpiAwareness();
        GT_HWND = prm.hFocusWindow;
        Hooks::WndProc_O = (WNDPROC)SetWindowLongPtr(GT_HWND, GWLP_WNDPROC, (LONG_PTR)WndProc_Hook);
        IsImguiInit = true;
        ImGui::GetIO().Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 12, NULL, ImGui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon());
        std::cout << "Bro got inited\n";
    }
    
    Gui::RenderGui();

    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());


label:
    return Hooks::D3D9_EndScene_Tramp(this_);
}

LRESULT __stdcall WndProc_Hook(HWND hwnd, UINT uMsg, WPARAM Wparam, LPARAM lParam) {
    ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, Wparam, lParam);
    return CallWindowProc(Hooks::WndProc_O, hwnd, uMsg, Wparam, lParam);
}
