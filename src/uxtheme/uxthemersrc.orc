/* $Id: uxthemersrc.orc,v 1.1 2002-04-10 18:35:19 bird Exp $ */

#include "winuser.h"
#include "odinrsrc.h"


/////////////////////////////////////////////////////////////////////////////
//
// Version (based on NT 4 SP3) - bad version numbers!
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
            VALUE "FileDescription", "UXTHEME System dll\0"
            VALUE "FileVersion", "4.57\0"
            VALUE "InternalName", "UXTHEME\0"
            VALUE "LegalCopyright", "Copyright (C) 1999-2000\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "UXTHEME.DLL\0"
            VALUE "ProductName", "Odin32 - UXTHEME\0"
            VALUE "ProductVersion", "4.57\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END
