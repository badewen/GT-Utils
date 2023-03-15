#include "Gui.h"

#include "ImGuI/imgui_impl_dx9.h"
#include "ImGuI/imgui_impl_win32.h"
#include "ImGuI/imgui.h"
#include "Hooks.h"
#include "Utils.h"
#include "Hook_Impl.h"
#include "PacketLogBuf.h"
#include <string>

#include <iostream>

#include "ImGuIAl/imguial_button.h"
#include "Hooks.h"

#define TAB_SAMELINE     ImGui::SameLine(0.0f, 3.0f)

#define MAIN_HACK_FRAME "main_hack#frame"

#define PACKET_LOGS std::string{"packet_logs"}
#define PACKET_LOGS_SELECTON_LIST PACKET_LOGS + "#selection_list"

enum class ActiveTab_Enum {
    Debug,
    Spoof,
    Packet_Logs
};

bool Gui::ChangeMac = false;
bool Gui::ChangeIp = false;
bool Gui::ChangeUserId = false;

bool Gui::SpoofMac = false;
bool Gui::SpoofIp = false;
bool Gui::SpoofUserId = false;

float Gui::SpoofedVersion = 4.17;

Timer timer{};

ActiveTab_Enum current_tab = ActiveTab_Enum::Spoof;

void Gui::RenderGui() {
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX9_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Begin("Utilities");

    TAB_SAMELINE;
    if (ImGuiAl::Button("Spoof", current_tab == ActiveTab_Enum::Spoof)) {
        current_tab = ActiveTab_Enum::Spoof;
    }
    TAB_SAMELINE;
    if (ImGuiAl::Button("Debug", current_tab == ActiveTab_Enum::Debug)) {
        current_tab = ActiveTab_Enum::Debug;
    }
    TAB_SAMELINE;
    if (ImGuiAl::Button("Packet Logs", current_tab == ActiveTab_Enum::Packet_Logs)) {
        current_tab = ActiveTab_Enum::Packet_Logs;
    }

    switch (current_tab)
    {
    case ActiveTab_Enum::Debug:
        __render_debug_tab();
        break;

    case ActiveTab_Enum::Spoof:
        __render_spoof_tab();
        break;

    case ActiveTab_Enum::Packet_Logs:
        __render_packet_logs_tab();

        break;

    default:
        break;
    }

    ImGui::End();

    ImGui::EndFrame();

    ImGui::Render();
}

void Gui::__render_debug_tab() {
    ImGui::BeginChild(MAIN_HACK_FRAME, ImVec2(0, 0), true);

    ImGui::Text("Hwnd : %s", Utils::to_hex((uint64_t)GT_HWND));
    ImGui::Text("EndScene addr : %s", Utils::to_hex(Hooks::D3D9_EndScene_Addr));
    //HandleIncomingPacket_Addr;
    //ENetPeerSend_Addr;
    //IsDebuggerPresent_Addr;
    //GetAdaptersAddresses_Addr;
    //D3D9_EndScene_Addr;
    ImGui::Text("HandleIncomingPacket addr : %s", Utils::to_hex(Hooks::HandleIncomingPacket_Addr));
    ImGui::Text("EnetPeerSend addr : %s", Utils::to_hex(Hooks::ENetPeerSend_Addr));
    ImGui::Text("IsDebuggerPresent addr : %s", Utils::to_hex(Hooks::IsDebuggerPresent_Addr));
    ImGui::Text("GetAdaptersAddresses addr : %s", Utils::to_hex(Hooks::GetAdaptersAddresses_Addr));

    ImGui::EndChild();
}

void Gui::__render_spoof_tab() {
    ImGui::BeginChild(MAIN_HACK_FRAME, ImVec2(0, 0), true);

    ImGui::Checkbox("Spoof Mac", &SpoofMac);
    ImGui::Checkbox("Spoof Ip", &SpoofIp);
    ImGui::Checkbox("Spoof UserId", &SpoofUserId);

    ImGui::Text("Version Spoof");
    ImGui::SameLine(0, 10);
    ImGui::InputFloat("", &SpoofedVersion);
    ImGui::SameLine(0, 10);
    if (ImGui::Button("-")) {
        SpoofedVersion -= 0.01;
    }
    ImGui::SameLine(0, 5);
    if (ImGui::Button("+")) {
        SpoofedVersion += 0.01;
    }

    if (ImGui::Button("Change Mac Address")) {
        ChangeMac = true;
    }
    ImGui::SameLine(0.0f, 30.0f);
    ImGui::Text("Generated mac : %s", Generated_Mac.c_str());
    //ImGui::SameLine(0.0f, 20);

    if (ImGui::Button("Change Ip Address")) {
        ChangeIp = true;
    }
    ImGui::SameLine(0.0f, 30.0f);
    ImGui::Text("Generated Ip : %s", Generated_Ip.c_str());

    if (ImGui::Button("Change User ID")) {
        ChangeUserId = true;
    }
    ImGui::SameLine(0.0f, 30.0f);
    ImGui::Text("Generated User ID : %s", Generated_UserId.c_str());

    ImGui::EndChild();
}

void Gui::__render_packet_logs_tab() {
    ImGui::BeginChild(MAIN_HACK_FRAME, { 0,0 }, true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

    ImGuiListClipper clipper;
    clipper.Begin(PacketLogBuf::GetProcessedQueue().size());

    while (clipper.Step()) {
        for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++) {
            ImGui::TextUnformatted(PacketLogBuf::GetProcessedQueue().at(line_no).c_str());
        }
    }

    ImGui::EndChild();
}