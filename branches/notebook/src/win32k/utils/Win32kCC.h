/* $Id: Win32kCC.h,v 1.6 2001-02-02 08:41:31 bird Exp $
 *
 *
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _Win32kCC_h_
#define _Win32kCC_h_

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "Dialog.h"

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/

/*
 * String IDs.
 */
#define  IDS_ERR_DIALOGLOAD             1
#define  IDS_ERR_MALLOC_FAILED          2
#define  IDS_ERR_SET_INSTACEDATA        3
#define  IDS_ERR_WIN32K_NOT_LOADED      4
#define  IDS_ERR_WIN32K_OPEN_FAILED     5
#define  IDS_ERR_QUERYOPTIONSTATUS      6
#define  IDS_ERR_NO_COM_RADIOBUTTON     7
#define  IDS_ERR_NO_PE_RADIOBUTTON      8
#define  IDS_ERR_NO_PE_ONEOBJECT        9
#define  IDS_ERR_INVALID_INFOLEVEL      10
#define  IDS_ERR_INVALID_MAXHEAPSIZE    11
#define  IDS_ERR_SETPOPTIONS            12
#define  IDS_ERR_FOPEN_FAILED           13
#define  IDS_FSIZE_FAILED               14
#define  IDS_MALLOC_FAILED              15
#define  IDS_FWRITE_FAILED              16


/*
 * Message IDs.
 */
#define  IDM_INFO_DIRTY                 1
#define  IDM_CONFIGSYS_UPDATED          2

#endif
