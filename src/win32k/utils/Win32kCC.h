/* $Id: Win32kCC.h,v 1.6.2.1 2001-02-17 08:57:51 bird Exp $
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
#define  IDS_ERR_SET_INSTANCEDATA       3
#define  IDS_ERR_ADD_NTBK_PAGE_LOAD     4
#define  IDS_ERR_ADD_NTBK_PAGE_INSERT   5
#define  IDS_ERR_ADD_NTBK_PAGE_SET      6
#define  IDS_ERR_WIN32K_NOT_LOADED      7
#define  IDS_ERR_WIN32K_OPEN_FAILED     8
#define  IDS_ERR_QUERYOPTIONSTATUS      9
#define  IDS_ERR_NO_COM_RADIOBUTTON     10
#define  IDS_ERR_NO_PE_RADIOBUTTON      11
#define  IDS_ERR_NO_PE_ONEOBJECT        12
#define  IDS_ERR_INVALID_INFOLEVEL      13
#define  IDS_ERR_INVALID_MAXHEAPSIZE    14
#define  IDS_ERR_SETPOPTIONS            15
#define  IDS_ERR_FOPEN_FAILED           16
#define  IDS_FSIZE_FAILED               17
#define  IDS_MALLOC_FAILED              18
#define  IDS_FWRITE_FAILED              19
#define  IDS_ERR_FAILED_TO_LOAD_DLGT    20
#define  IDS_ERR_FAILED_NTBK_BUTTONS    21
#define  IDS_ERR_ASSERT                 22


/*
 * Message IDs.
 */
#define  IDM_INFO_DIRTY                 1
#define  IDM_CONFIGSYS_UPDATED          2

#endif
