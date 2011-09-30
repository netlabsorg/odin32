/* $Id: msvcrtrsrc.orc,v 1.5 2003-04-10 10:28:06 sandervl Exp $ */

#include "winuser.h"
#include "odinrsrc.h"

/////////////////////////////////////////////////////////////////////////////
//
// Version (MSVCRT.DLL from NT 4, SP3)
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
            VALUE "FileVersion", "6.00.8397.0\0"
            VALUE "InternalName", "MSVCRT\0"
            VALUE "LegalCopyright", "Copyright (C) 1999\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "MSVCRT.DLL\0"
            VALUE "ProductName", "Odin32 - MSVCRT\0"
            VALUE "ProductVersion", "6.00.8397.0\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END
