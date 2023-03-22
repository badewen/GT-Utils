// appended Virt_Impl to avoid collision with MinHook's hook.c. (MSVC is dumb)
// Hook.h is just an "interface"

// use MinHook as other hooking libraries are experimental.

#define USE_MINHOOK
//#define USE_MHOOK
//#define USE_DETOURS

#if defined(USE_MINHOOK)
#include "Hooking_Backends/Hook_MinHook_Impl.h"
#elif defined(USE_MHOOK)
#include "Hooking_Backends/Hook_MHook_Impl.h"
#elif defined(USE_DETOURS)
#include "Hooking_Backends/Hook_Detours_Impl.h"
#endif