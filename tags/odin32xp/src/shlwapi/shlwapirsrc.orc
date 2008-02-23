/* $Id: shlwapirsrc.orc,v 1.2 2000-08-24 12:00:50 sandervl Exp $ */

#include "winuser.h"
#include "odinrsrc.h"


/////////////////////////////////////////////////////////////////////////////
//
// Version (based on NT 4 SP6 + IE 5.01)
//

1 VERSIONINFO
 FILEVERSION    0x05, 0x00, 0x0B67, 0x18A0
 PRODUCTVERSION 0x05, 0x00, 0x0B67, 0x18A0
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
            VALUE "FileDescription", "SHLWAPI System dll\0"
            VALUE "FileVersion", "5.00.2919.6304\0"
            VALUE "InternalName", "SHLWAPI\0"
            VALUE "LegalCopyright", "Copyright (C) 1999-2000\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "SHLWAPI.DLL\0"
            VALUE "ProductName", "Odin32 - SHLWAPI\0"
            VALUE "ProductVersion", "5.00.2919.6304\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END
