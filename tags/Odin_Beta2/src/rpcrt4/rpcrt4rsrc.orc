/* $Id: rpcrt4rsrc.orc,v 1.1 2000-04-02 22:02:57 davidr Exp $ */

#include "winuser.h"
#include "odinrsrc.h"


/////////////////////////////////////////////////////////////////////////////
//
// Version (based on oleaut32 with the edges filed off!)
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
            VALUE "FileDescription", "RPCRT4 System dll\0"
            VALUE "FileVersion", "4.00\0"
            VALUE "InternalName", "RPCRT4.DLL\0"
            VALUE "LegalCopyright", "Copyright (C) 2000\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "RPCRT4.DLL\0"
            VALUE "ProductName", "Odin32 - RPCRT4\0"
            VALUE "ProductVersion", "4.00\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x409, 0x04E4
    END
END

