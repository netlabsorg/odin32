/* $Id: api.h,v 1.3 2001-01-21 07:52:46 bird Exp $
 *
 * API Overload Init and Helper Function - public header.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*NOINC*/
#ifndef _API_H_
#define _API_H_
/*INC*/


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define API_DOSALLOCMEM_ANY_OBJ                     0
#define API_DOSALLOCSHAREDMEM_ANY_OBJ               1
#define API_MAX                                     API_DOSALLOCSHAREDMEM_ANY_OBJ
#define API_CENTRIES                                (API_MAX + 1)

/*NOINC*/
APIRET _Optlink APIInit(void);
BOOL _Optlink   APIQueryEnabled(int iApi, USHORT usCS, LONG ulEIP);
/*INC*/

/*NOINC*/
#endif
/*INC*/
