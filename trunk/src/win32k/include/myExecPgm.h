/* $Id: myExecPgm.h,v 1.1 2000-02-19 08:41:10 bird Exp $
 *
 * Desclarations for buffer.asm and mytkExecPgm.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _myExecPgm_h_
#define _myExecPgm_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define CB_BUFFER   1536                /* This is to be updated in the files buffer.asm and tkExecPgm too! */
#define CCH_FILENAME 261                /* This is to be updated in the files buffer.asm and tkExecPgm too! */


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#pragma pack(1)
typedef struct _EXECPGMBUFFER
{
    char szFilename[CCH_FILENAME];          /* Executable filename. */
    char achArgs[CB_BUFFER - CCH_FILENAME]; /* collection of ASCIIZ which ends with an empty string */
} EXECPGMBUFFER, *PEXECPGMBUFFER;
#pragma pack()


/*******************************************************************************
*   Functions Prototypes                                                       *
*******************************************************************************/
PEXECPGMBUFFER _Optlink QueryBufferPointerFromFilename(const char *pszFilename);

#endif
