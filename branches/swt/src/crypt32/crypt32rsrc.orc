/* $Id: riched32rsrc.orc,v 1.1 2000/05/10 13:17:27 sandervl Exp $ */

#include "winuser.h"
#include "odinrsrc.h"

#include "cryptres.h"
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
            VALUE "FileDescription", "CRYPT32 System dll\0"
            VALUE "FileVersion", "4.00.993.4"
            VALUE "InternalName", "CRYPT32\0"
            VALUE "LegalCopyright", "Copyright (C) 2008\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "CRYPT32.DLL\0"
            VALUE "ProductName", "Odin32 - CRYPT32\0"
            VALUE "ProductVersion", "4.0\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END

#include "crypt32_En.rc"
