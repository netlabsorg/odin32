/* $Id: ole32rsrc.orc,v 1.3 2004-01-30 22:11:41 bird Exp $ */

#include "windef.h"
#include "winuser.h"
#include "winnls.h"
#include "odinrsrc.h"


/*
 * Everything that does not depend on language,
 * like textless bitmaps etc, go into the
 * neutral language. This will prevent them from
 * being duplicated for each language.
 */
LANGUAGE LANG_NEUTRAL, SUBLANG_NEUTRAL

0 CURSOR nodrop.cur
1 CURSOR drag_move.cur
2 CURSOR drag_copy.cur
3 CURSOR drag_link.cur


/*
 * Everything specific to any language goes
 * in one of the specific files.
 * Note that you can and may override resources
 * which also have a neutral version. This is to
 * get localized bitmaps for example.
 */

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
            VALUE "FileDescription", "OLE32 System dll\0"
            VALUE "FileVersion", "4.00\0"
            VALUE "InternalName", "OLE32\0"
            VALUE "LegalCopyright", "Copyright (C) 1999-2000\0"
            VALUE "LegalTrademarks", "\0"
            VALUE "OriginalFilename", "OLE32.DLL\0"
            VALUE "ProductName", "Odin32 - OLE32\0"
            VALUE "ProductVersion", "4.00\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
	VALUE "Translation", 0x409, 0x04E4
    END
END

