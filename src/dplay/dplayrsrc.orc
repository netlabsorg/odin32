/* $Id: dplayrsrc.orc,v 1.3 2000-08-18 21:41:24 sandervl Exp $ */

#include "winuser.h"
#include "odinrsrc.h"


/////////////////////////////////////////////////////////////////////////////
//
// Version (based on NT 4 SP6)
//

1 VERSIONINFO
 FILEVERSION 0x04, 0x00, 0x055B, 0x0001
 PRODUCTVERSION 0x04, 0x00, 0x055B, 0x0001
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
            VALUE "FileDescription", "DPLAY System dll\0"
            VALUE "FileVersion", "4.00\0"
            VALUE "InternalName", "DPLAY\0"
            VALUE "LegalCopyright", "Copyright (C) 1999-2000\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "DPLAY.DLL\0"
            VALUE "ProductName", "Odin32 - DPLAY\0"
            VALUE "ProductVersion", "4.00\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END
