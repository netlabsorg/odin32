/* $Id: quartzrsrc.orc,v 1.1 2001-08-19 14:02:13 sandervl Exp $ */

#include "winuser.h"
#include "odinrsrc.h"


/////////////////////////////////////////////////////////////////////////////
//
// Version (based on NT 4 SP6 gdi32.dll)
//

1 VERSIONINFO
 FILEVERSION 0x04, 0x00, 0x0565, 0x012A
 PRODUCTVERSION 0x04, 0x00, 0x0565, 0x012A
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
            VALUE "FileDescription", "QUARTZ System dll\0"
            VALUE "FileVersion", "4.00\0"
            VALUE "InternalName", "QUARTZ\0"
            VALUE "LegalCopyright", "Copyright (C) 1999-2001\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "QUARTZ.DLL\0"
            VALUE "ProductName", "Odin32 - QUARTZ\0"
            VALUE "ProductVersion", "4.00\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END
