/* $Id: api.h,v 1.3.2.1 2002-04-01 10:01:01 bird Exp $
 *
 * API Overload Init and Helper Function - public header.
 *
 * Copyright (c) 2001-2002 knut st. osmundsen (bird@anduin.net)
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
