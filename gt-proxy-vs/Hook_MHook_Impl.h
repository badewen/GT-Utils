#pragma once

// tricky implementation :skull:

#include "MHook/mhook-lib/mhook.h"
#include "Hook_Virt.h"

#include <vector>

struct HOOK_INFO_EX {
    void* Func;
    void* Address;
    void** Tramp_Addr;
    bool IsActive;
};

typedef enum MHOOK_STATUS {
    MHOOK_STATUS_FAIL,
    MHOOK_STATUS_HOOK_ADD_SUCCESS,
    MHOOK_STATUS_HOOK_ENABLE_SUCCESS,
    MHOOK_STATUS_HOOK_ENABLE_FAIL,
    MHOOK_STATUS_HOOK_LIST_NOTFOUND,
    MHOOK_STATUS_HOOK_PARTIAL_SUCCESS
};

std::vector<HOOK_INFO_EX> hook_list;

void Hook::Init() {
    hook_list = {};
}

uint32_t Hook::AddHook(void* addr, void* func, void** tramp) {
    
    HOOK_INFO_EX inf{};

    inf.Address = addr;
    inf.Func = func;
    inf.Tramp_Addr = tramp;
    inf.IsActive = false;

    hook_list.push_back(inf);
    
    return MHOOK_STATUS_HOOK_ADD_SUCCESS;
}

uint32_t Hook::EnableHook(void* hook) {
    bool status = false;

    for (HOOK_INFO_EX& inf : hook_list) {
        if (inf.Address == hook && !inf.IsActive) {
            inf.IsActive = true;

            // this library output tramp address to target address variable
            // so this variable will become tramp address from target address
            void* to_hook_and_tramp = inf.Address;

            status = Mhook_SetHook(&to_hook_and_tramp, inf.Func);

            if (!status) {
                inf.IsActive = false;
                return MHOOK_STATUS_HOOK_ENABLE_FAIL;
            }

            *inf.Tramp_Addr = to_hook_and_tramp;
            
            return MHOOK_STATUS_HOOK_ENABLE_SUCCESS;

        }
    }

    return MHOOK_STATUS_HOOK_LIST_NOTFOUND;

}

uint32_t Hook::EnableAllHook() {
    uint32_t status = 0;

    for (HOOK_INFO_EX inf : hook_list) {
        if (EnableHook(inf.Address)) {
            status++;
        }
    }

    if (status > 0) {
        return MHOOK_STATUS_HOOK_PARTIAL_SUCCESS;
    }
    return MHOOK_STATUS_HOOK_ENABLE_FAIL;
}

void Hook::DisableHook(void* hook) {
    bool status = false;

    for (HOOK_INFO_EX& inf : hook_list) {
        if (inf.Address == hook && inf.IsActive) {
            inf.IsActive = false;

            status = Mhook_Unhook(inf.Tramp_Addr);

            if (!status) {
                inf.IsActive = true;
                return;
            }
            return;

        }
    }

    return;
}

void Hook::DisableAllHook() {
    for (HOOK_INFO_EX inf : hook_list) {
        DisableHook(inf.Address);
    }

    return;
}

void Hook::UnInit() {

}

std::string Hook::StatusToString(uint32_t err) {
    switch (err)
    {

    case MHOOK_STATUS_FAIL:
        return "MHOOK_STATUS_FAIL";

    case MHOOK_STATUS_HOOK_ADD_SUCCESS:
        return "MHOOK_STATUS_HOOK_ADD_SUCCESS";

    case MHOOK_STATUS_HOOK_ENABLE_FAIL:
        return "MHOOK_STATUS_HOOK_ENABLE_FAIL";

    case MHOOK_STATUS_HOOK_ENABLE_SUCCESS:
        return "MHOOK_STATUS_HOOK_ENABLE_SUCCESS";

    case MHOOK_STATUS_HOOK_LIST_NOTFOUND:
        return "MHOOK_STATUS_HOOK_LIST_NOTFOUND";

    case MHOOK_STATUS_HOOK_PARTIAL_SUCCESS:
        return "MHOOOK_STATUS_HOOK_PARTIAL_SUCCESS";

    default:
        return "MHOOK_ERR_UNK";
        break;
    }
}
