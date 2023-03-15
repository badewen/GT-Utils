#pragma once
namespace Gui
{
    extern void RenderGui();

    extern void __render_debug_tab();
    extern void __render_spoof_tab();
    extern void __render_packet_logs_tab();

    extern bool ChangeMac;
    extern bool ChangeIp;
    extern bool ChangeUserId;

    extern bool SpoofMac;
    extern bool SpoofIp;
    extern bool SpoofUserId;

    extern float SpoofedVersion;
};
