#if !defined (KBD32_UTILS_H)
#define KBD32_UTILS_H

#include <os2.h>

extern "C" {
/// returns the parent pid of a given pid
ULONG getParentPID(ULONG pid);

/// returns the window associated with the current Vio wondow
HWND VIOWindowClient();

///
HWND VIOWindowFrame();

}
#endif
