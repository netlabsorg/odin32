/* $Id: olesvr32rsrc.orc,v 1.1 2000-03-06 23:38:54 bird Exp $ */

#include "winuser.h"
#include "odinrsrc.h"


/////////////////////////////////////////////////////////////////////////////
//
// Version (based on NT 4 SP3)
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
            VALUE "FileDescription", "Object Linking and Embedding Server Library\0"
            VALUE "FileVersion", "1.09\0"
            VALUE "InternalName", "OLESVR\0"
            VALUE "LegalCopyright", "Copyright (C) 1999-2000\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "OLESVR32.DLL\0"
            VALUE "ProductName", "Odin Object Linking and Embedding Libraries for OS/2\0"
            VALUE "ProductVersion", "3.10.0.071\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END
