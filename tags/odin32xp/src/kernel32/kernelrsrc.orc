/* $Id: kernelrsrc.orc,v 1.4 2003-07-28 11:35:31 sandervl Exp $ */

#include "winuser.h"
#include "odinrsrc.h"


/////////////////////////////////////////////////////////////////////////////
//
// Version (based on NT 4 SP6 kernel32.dll)
//

1 VERSIONINFO
 FILEVERSION 0x04, 0x00, 0x0565, 0x012C
 PRODUCTVERSION 0x04, 0x00, 0x0565, 0x012C
 FILEFLAGSMASK 0x3fL
 FILEFLAGS 0xaL
 FILEOS 0x10001L
 FILETYPE 0x1L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040904E4"
        BEGIN
            VALUE "Comments", "Odin32 System Dll\0"
            VALUE "CompanyName", "Odin Team\0"
            VALUE "FileDescription", "KERNEL32 System dll\0"
            VALUE "FileVersion", "4.00\0"
            VALUE "InternalName", "KERNEL32\0"
            VALUE "LegalCopyright", "Copyright (C) 1999\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "KERNEL32.DLL\0"
            VALUE "ProductName", "Odin32 - KERNEL32\0"
            VALUE "ProductVersion", "4.00\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END

#include "nls\locale_rc.rc"
#include "messages\winerr_enu.mc.rc"
