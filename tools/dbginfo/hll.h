/* $Id: hll.h,v 1.1 2000-03-24 18:13:45 bird Exp $
 *
 * HLL definitions.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _HLL_h_
#define _HLL_h_

#pragma pack(1)


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/

/*
 * Directory entry types.
 */
#define HLL_DE_MODULES          0x0101  /* Filename */
#define HLL_DE_PUBLICS          0x0102  /* Public symbols */
#define HLL_DE_TYPES            0x0103  /* Types */
#define HLL_DE_SYMBOLS          0x0104  /* Symbols */
#define HLL_DE_LIBRARIES        0x0106  /* Libraries */
#define HLL_DE_SRCLINES         0x0105  /* Line numbers - (IBM C/2 1.1) */
#define HLL_DE_SRCLNSEG         0x0109  /* Line numbers - (MSC 6.00) */
#define HLL_DE_IBMSRC           0x010B  /* Line numbers - (IBM HLL) */



/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/

/*
 * HLL HDR Entry.
 */
typedef struct _HLLDirEntry
{
    unsigned short  usType;
    unsigned short  iMod;
    unsigned long   off;
    unsigned long   cb;
} HLLDIRENTRY, *PHLLDIRENTRY;



/*
 * HLL Directory.
 */
typedef struct _HLLDirectory
{
    unsigned long   cEntries;
    HLLDIRENTRY     aEntries[1];
} HLLDIR, *PHLLDIR;






#pragma pack()

#endif
