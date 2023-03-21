#pragma once

#include <stdint.h>
#include <string>

// this is specifically designed for MinHook. 
// other hooking library probably won't work properly

// this is an "interface" or front end to communicate with backend hooking library.

class Hook
{
public:
    static void Init();
    
    // EnableHook / EnableAllHook must be called after AddHook to avoid UB.
    // wont cause any UB if you are using MinHook implementation though.
    static uint32_t AddHook(void* addr, void* func, void** tramp);
    static uint32_t EnableHook(void* hook);
    static uint32_t EnableAllHook();

    static void DisableHook(void* hook);
    static void DisableAllHook();
    static void UnInit();

    static std::string StatusToString(uint32_t code);
};

