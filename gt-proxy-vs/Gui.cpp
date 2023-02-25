#include "Gui.h"

#include "ImGuI/imgui_impl_dx9.h"
#include "ImGuI/imgui_impl_win32.h"
#include "ImGuI/imgui.h"
#include "Hooks.h"
#include "Utils.h"

#include <iostream>

#include "ImGuIAl/imguial_button.h"
#include "Hooks.h"

#define TAB_SAMELINE     ImGui::SameLine(0.0f, 3.0f)

enum class ActiveTab_Enum {
    Debug,
    Spoof
};

ActiveTab_Enum current_tab = ActiveTab_Enum::Spoof;

void Gui::RenderGui() {
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX9_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Utilities");

    TAB_SAMELINE;
    if (ImGuiAl::Button("Debug", current_tab == ActiveTab_Enum::Debug)) {
        current_tab = ActiveTab_Enum::Debug;
    }
    TAB_SAMELINE;
    if (ImGuiAl::Button("HAAAA", current_tab == ActiveTab_Enum::Spoof)) {
        current_tab = ActiveTab_Enum::Spoof;
    }

    ImGui::BeginChild("wtf", ImVec2(0, 0), true);
        
        switch (current_tab)
        {

        case ActiveTab_Enum::Debug:
            __render_debug_tab();
            break;

        case ActiveTab_Enum::Spoof:
            break;

        default:
            break;
        }

    ImGui::EndChild();

    ImGui::End();

    ImGui::EndFrame();

    ImGui::Render();
}

void Gui::__render_debug_tab() {
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
}
