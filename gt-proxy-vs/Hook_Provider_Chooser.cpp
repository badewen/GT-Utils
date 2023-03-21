// appended Virt_Impl to avoid collision with MinHook's hook.c. (MSVC is dumb)
// Hook.h is just an "interface"

#define USE_MINHOOK


#ifdef USE_MINHOOK
#include "Hook_MinHook_Impl.h"
#else
#include "Hook_MHook_Impl.h"
#endif
