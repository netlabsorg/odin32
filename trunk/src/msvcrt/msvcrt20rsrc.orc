/* $Id: msvcrt20rsrc.orc,v 1.5 2003-04-10 10:28:05 sandervl Exp $ */

#include "winuser.h"
#include "odinrsrc.h"

/////////////////////////////////////////////////////////////////////////////
//
// Version (MSVCRT20.DLL from NT 4, SP3)
//

1 VERSIONINFO
 FILEVERSION ODIN_FILEVERSION
 PRODUCTVERSION ODIN_PRODUCTVERSION
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
            VALUE "FileDescription", "C/C++ Runtime library\0"
            VALUE "FileVersion", "2.11.000\0"
            VALUE "InternalName", "MSVCRT20\0"
            VALUE "LegalCopyright", "Copyright (C) 1999\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "MSVCRT20.DLL\0"
            VALUE "ProductName", "Odin32 - MSVCRT20\0"
            VALUE "ProductVersion", "2.10.000\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END
