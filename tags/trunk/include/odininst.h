/* $Id: odininst.h,v 1.1 2000-04-29 18:25:02 sandervl Exp $ */

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

//******************************************************************************
//Called from the WarpIn install program to create the desktop directories and 
//to setup the registry
//******************************************************************************
BOOL InitSystemAndRegistry();

#endif //__ODININST_H__

