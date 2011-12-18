#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winreg.h>
#include <winnt.h>
#include <wincon.h>
#include <winthunk.h>
#include <winuser.h>
#include <ddeml.h>
#include <dde.h>
#include <winnls.h>
#include <ntddk.h>
#include <heapstring.h>
#include <version.h>

#define DBG_LOCALLOG    DBG_trace
#include <dbglocal.h>

#define DBGWRAP_MODULE "VERSION"
#include <dbgwrap.h>





#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_info

DEBUGWRAP16(GetFileVersionInfoA)
DEBUGWRAP8(GetFileVersionInfoSizeA)
DEBUGWRAP8(GetFileVersionInfoSizeW)
DEBUGWRAP16(GetFileVersionInfoW)
DEBUGWRAP32(VerFindFileA)
DEBUGWRAP32(VerFindFileW)
DEBUGWRAP16(VerQueryValueA)
DEBUGWRAP16(VerQueryValueW)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_install

DEBUGWRAP32(VerInstallFileA)
DEBUGWRAP32(VerInstallFileW)

