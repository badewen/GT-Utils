#include "Hook_Impl.h"

#include <iostream>

#define LOGI std::cout

int __cdecl enet_peer_send_hook(ENetPeer* peer, enet_uint8 channelID, ENetPacket* packet) {

    // lol, i think i read too many of ida decompiler code.
    if (*(uint32_t*)packet->data == 2) {
        char* buff = (char*)malloc(packet->dataLength - 4 + 1);

        memset(buff, 0, packet->dataLength - 4 + 1);
        memcpy(buff, packet->data + 4, packet->dataLength - 4);

        std::string bruh = { buff };
        free(buff);
        try {
            if (!modify_text_packet(bruh, "4.14", "game_version|")) {
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
    for (int i = 0; i < packet->dataLength; i++) {
        printf("0x%02x ", ((uint8_t*)(packet->data))[i]);
    }
    LOGI << "\"\n";

    LOGI << "in string : \"";

    for (int i = 0; i < packet->dataLength; i++) {
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

int __cdecl enet_host_service_hook(ENetHost* host, ENetEvent* event, enet_uint32 timeout) {

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


    for (int i = 0; i < enet_event->packet->dataLength; i++) {
        printf("0x%02x ", ((uint8_t*)enet_event->packet->data)[i]);
    }
    LOGI << "\n";

    LOGI << "in string : \"";

    for (int i = 0; i < enet_event->packet->dataLength; i++) {
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