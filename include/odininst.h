/* $Id: odininst.h,v 1.2 2000-12-30 10:17:45 sandervl Exp $ */

/*
 * Exported installation procedure in kernel32
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __ODININST_H__
#define __ODININST_H__

#define ODINFONTSECTION "Font Mapping"

//******************************************************************************
//Called from the WarpIn install program to create the desktop directories and 
//to setup the registry
//******************************************************************************
BOOL InitSystemAndRegistry();

#endif //__ODININST_H__

