/* $Id: shdocvwrsrc.orc,v 1.2 2004-07-06 16:57:18 sandervl Exp $ */

#include "winuser.h"
#include "odinrsrc.h"


/////////////////////////////////////////////////////////////////////////////
//
// Version (based on NT 4 SP6)
//

1 VERSIONINFO
 FILEVERSION 0x05, 500, 0x0565, 0x0119
 PRODUCTVERSION 0x05, 50, 0x0565, 0x0119
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
            VALUE "FileDescription", "SHDOCVW System dll\0"
            VALUE "FileVersion", "5.50.0.0\0"
            VALUE "InternalName", "SHDOCVW\0"
            VALUE "LegalCopyright", "Copyright (C) 1999-2001\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "SHDOCVW.DLL\0"
            VALUE "ProductName", "Odin32 - SHDOCVW\0"
            VALUE "ProductVersion", "5.50\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END
