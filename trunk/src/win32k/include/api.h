/* $Id: api.h,v 1.1 2001-01-20 15:47:08 bird Exp $
 *
 * API Overload Init and Helper Function - public header.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _API_H_
#define _API_H_


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define API_DOSALLOCMEM_ANY_OBJ                     0
#define API_DOSALLOCSHAREDMEM_ANY_OBJ               1
#define API_MAX                                     2


void _Optlink   APIInit(void);
BOOL _Optlink   APIQueryEnabled(int iAPI, USHORT usCS, LONG ulEIP);


#endif
