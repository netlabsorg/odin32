/* $Id: glu32rsrc.orc,v 1.1 2000-03-06 23:33:48 bird Exp $ */

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
            VALUE "CompanyName", "Silicon Graphics, Inc\0"
            VALUE "FileDescription", "OpenGL Utility Library DLL\0"
            VALUE "FileVersion", "4.00\0"
            VALUE "InternalName", "glu32\0"
            VALUE "LegalCopyright", "Copyright (C) 2000 Silicon Graphics, Inc\0"
            VALUE "LegalTrademarks", "Copryright (C) Silicon Graphics, Inc\0"
            VALUE "OriginalFilename", "glu32\0"
            VALUE "ProductName", "Odin32 - GLU32\0"
            VALUE "ProductVersion", "4.00\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END
