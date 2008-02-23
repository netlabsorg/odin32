/* $Id: */

#include "winuser.h"
#include "odinrsrc.h"


/////////////////////////////////////////////////////////////////////////////
//
// Version (based on NT 4 SP6)
//

1 VERSIONINFO                                           //?? kso
 FILEVERSION 0x04, 0x00, 0x0565, 0x012C                 //?? kso
 PRODUCTVERSION 0x04, 0x00, 0x0565, 0x012C              //?? kso
 FILEFLAGSMASK 0x3fL                                    //?? kso
 FILEFLAGS 0xaL                                         //?? kso
 FILEOS 0x10001L                                        //?? kso
 FILETYPE 0x1L                                          //?? kso
 FILESUBTYPE 0x0L                                       //?? kso
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040704B0"
        BEGIN
            VALUE "Comments", "Odin32 System Dll\0"
            VALUE "CompanyName", "Odin Team\0"
            VALUE "FileDescription", "VDMDBG System dll\0"
            VALUE "FileVersion", "4.00\0"
            VALUE "InternalName", "VDMDBG\0"
            VALUE "LegalCopyright", "Copyright (C) 2001\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "VDMDBG.DLL\0"
            VALUE "ProductName", "Odin32 - VDMDBG\0"
            VALUE "ProductVersion", "4.00\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END

