#pragma once

#include "../Hook_Virt.h"
#include "MinHook/include/MinHook.h"

void Hook::Init() {
    MH_Initialize();
}


uint32_t Hook::AddHook(void* addr, void* func, void** tramp) {
    uint32_t ret = 0;
    ret = MH_CreateHook(addr, func, tramp);

    return ret;
}

uint32_t Hook::EnableHook(void* hook) {
    return MH_EnableHook(hook);
}

uint32_t Hook::EnableAllHook() {
    return MH_EnableHook(MH_ALL_HOOKS);
}

void Hook::DisableHook(void* hook) {
    MH_DisableHook(hook);
}

void Hook::DisableAllHook() {
    MH_DisableHook(MH_ALL_HOOKS);
}

void Hook::UnInit() {
    MH_Uninitialize();
}

std::string Hook::StatusToString(uint32_t err) {
    return MH_StatusToString((MH_STATUS)err);
}
