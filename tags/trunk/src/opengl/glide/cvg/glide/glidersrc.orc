/*
** THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
** PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
** TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
** INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE 
** DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com). 
** THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
** EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
** FULL TEXT OF THE NON-WARRANTY PROVISIONS.  
** 
** USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
** RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
** TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
** AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
** SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
** THE UNITED STATES.  
** 
** COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
**
** $Header: /home/ktk/tmp/odin/odin32xp/src/opengl/glide/cvg/glide/glidersrc.orc,v 1.1 2000-03-06 23:33:43 bird Exp $
** $Log: glidersrc.orc,v $
** Revision 1.1  2000-03-06 23:33:43  bird
** Makefiles are updated to new style.
** Odin32 resource files are renamed to *.orc and most have changed name to avoid
** name clashes for object files.
**
** Revision 1.1  2000/02/25 00:37:40  sandervl
** Created Voodoo 2 dir
**
** Revision 1.1  2000/02/18 10:43:08  sandervl
** Created Voodoo 1 Glide dir
**
*/

#define OFFICIAL   1 
#define FINAL      1 

#include <fxver.h>
#include "rcver.h"
//#include "fxbldno.h"

/////////////////////////////////////////////////////////////////////////////
//
// Version
//

1 VERSIONINFO
 FILEVERSION MANVERSION, MANREVISION, 0, 1
 PRODUCTVERSION MANVERSION, MANREVISION, 0, 1
 FILEFLAGSMASK 0x0030003FL
 FILEFLAGS (VER_PRIVATEBUILD|VER_PRERELEASE|VER_DEBUG)

 FILEOS VOS_DOS_WINDOWS32
 FILETYPE VFT_DRV
 FILESUBTYPE VFT2_DRV_INSTALLABLE
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040904E4"
        BEGIN
            VALUE "CompanyName", "3Dfx Interactive, Inc.\0"
            VALUE "FileDescription", "3Dfx Interactive, Inc. Glide DLL\0"
            VALUE "FileVersion", "2.53\0"
            VALUE "InternalName", "glide2x.dll\0"
            VALUE "LegalCopyright", "Copyright \251 3Dfx Interactive, Inc. 1997\0"
            VALUE "OriginalFilename", "glide2x.dll\0"
            VALUE "ProductName", "Glide(tm) for Voodoo Graphics\251 and OS/2 Warp\0"
            VALUE "ProductVersion", "2.53\0"
            VALUE "Graphics Subsystem", "Voodoo Graphics(tm)\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        /* the following line should be extended for localized versions */
        VALUE "Translation", 0x409, 1252
    END
END
