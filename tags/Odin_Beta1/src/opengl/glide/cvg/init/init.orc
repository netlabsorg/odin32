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
** $Header: /home/ktk/tmp/odin/2007/netlabs.cvs/odin32/src/opengl/glide/cvg/init/init.orc,v 1.1 2000-03-06 23:33:43 bird Exp $
** $Log: init.orc,v $
** Revision 1.1  2000-03-06 23:33:43  bird
** Makefiles are updated to new style.
** Odin32 resource files are renamed to *.orc and most have changed name to avoid
** name clashes for object files.
**
** Revision 1.1  2000/02/25 00:37:52  sandervl
** Created Voodoo 2 dir
**
*/

#define OFFICIAL   1 
#define FINAL      1 

#include <fxver.h>
#include "rcver.h"

/////////////////////////////////////////////////////////////////////////////
//
// Version
//
#define VERSIONNAME             "sst1init.dll\0"

VS_VERSION_INFO VERSIONINFO
 FILEVERSION MANVERSION, MANREVISION, 0, BUILD_NUMBER
 PRODUCTVERSION MANVERSION, MANREVISION, 0, BUILD_NUMBER
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
            VALUE "FileDescription", "3Dfx Interactive, Inc. InitCode DLL\0"
            VALUE "FileVersion", VERSIONSTR
            VALUE "InternalName", VERSIONNAME
            VALUE "LegalCopyright", "Copyright \251 3Dfx Interactive, Inc. 1996\0"
            VALUE "OriginalFilename", VERSIONNAME
            VALUE "ProductName", "InitCode for Voodoo Graphics\251 and Windows\256  95\0"
            VALUE "ProductVersion", VERSIONSTR
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        /* the following line should be extended for localized versions */
        VALUE "Translation", 0x409, 1252
    END
END
