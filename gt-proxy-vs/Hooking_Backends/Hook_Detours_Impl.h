
#include "../Hook_Virt.h"

#include <Windows.h>
#include "Detours/detours.h"
#include <vector>

// lmao this is similar to MHook except this one is official

struct HOOK_INFO_EX {
    void* addr;
    void* func;
    void** tramp_addr_ptr;
    bool isActive;
};

enum DETOURS_ERR {
    DETOURS_ERR_NO_ERR,
    DETOURS_ERR_FAIL,
    DETOURS_ERR_PARTIAL = -1
};

std::vector<HOOK_INFO_EX> hook_list{};

void Hook::Init() {
    hook_list = {};
}

uint32_t Hook::AddHook(void* addr, void* func, void** tramp) {
    HOOK_INFO_EX inf{};

    inf.addr = addr;
    inf.func = func;
    inf.tramp_addr_ptr = tramp;
    inf.isActive = false;

    hook_list.push_back(inf);

    return DETOURS_ERR_NO_ERR;
}

uint32_t Hook::EnableHook(void* hook) {

    for (HOOK_INFO_EX inf : hook_list) {
        if (inf.addr == hook && !inf.isActive) {
            void* addr_cpy = inf.addr;
            bool ret = false;

            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&addr_cpy, inf.func);
            ret = DetourTransactionCommit();

            *inf.tramp_addr_ptr = addr_cpy;

            return ret;
        }
    }
    return DETOURS_ERR_FAIL;
}

uint32_t Hook::EnableAllHook() {

    uint32_t pending_hook_count = 0, status = 0;

    for (HOOK_INFO_EX inf : hook_list) {
        if (Hook::EnableHook(inf.addr)) {
            status++;
        }
    }

    if (status > 0 && status < pending_hook_count) {
        return DETOURS_ERR_PARTIAL;
    }

    if (status == pending_hook_count) {
        return DETOURS_ERR_NO_ERR;
    }

    return DETOURS_ERR_FAIL;
}

void Hook::DisableHook(void* hook) {
    for (HOOK_INFO_EX inf : hook_list) {
        if (inf.addr == hook && inf.isActive) { 

            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourDetach(inf.tramp_addr_ptr, inf.addr);
            DetourTransactionCommit();
            inf.isActive = false;
        }
    }
}

void Hook::DisableAllHook() {
    for (HOOK_INFO_EX inf : hook_list) {
        DisableHook(inf.addr);
    }
}

void Hook::UnInit() {}

std::string Hook::StatusToString(uint32_t err) {
    switch (err)
    {
    case DETOURS_ERR_FAIL:
        return "DETOURS_ERR_FAIL";

    case DETOURS_ERR_NO_ERR: 
        return "DETOURS_ERR_NO_ERR";

    case DETOURS_ERR_PARTIAL:
        return DETOURS_STRINGIFY_(DETOURS_ERR_PARTIAL);

    default:
        return "Unk Code. Probably from Detour's library.";
        break;
    }
}
