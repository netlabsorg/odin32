/* $Id: krnlImportTable.h,v 1.2 2002-12-16 02:25:07 bird Exp $
 *
 * krnlImportTable definitions.
 *
 * Copyright (c) 1998-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __krnlImportTable_h__
#define __krnlImportTable_h__

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define NBR_OF_KRNLIMPORTS      114     /* When this is changed make sure to   */
                                        /* update the aImportTab in probkrnl.c */
                                        /* and make test faker in test.h and   */
                                        /* the appropriate fake.c or fakea.asm.*/
#define MAX_LENGTH_NAME         32      /* Max length for the function. */

/* Entry-Point Type flag */
#define EPT_PROC                0x00    /* procedure - overload procedure */
#define EPT_PROCH               0x01    /* procedure hybrid (import only) */
#define EPT_VAR                 0x02    /* variable/non-procedure         */
#define EPT_NOT_REQ             0x08    /* Not required flag. */
#define EPTNotReq(a)            (((a).fType & (EPT_NOT_REQ)) == EPT_NOT_REQ)
#define EPT_WRAPPED             0x40    /* Wrapped due - differs between builds */
#define EPTWrapped(a)           (((a).fType & (EPT_WRAPPED)) == EPT_WRAPPED)
#define EPT_32BIT               0x00    /* 32 bit entry-point  */
#define EPT_16BIT               0x80    /* 16 bit entry-point */
#define EPT_BIT_MASK            0x80    /* Mask bit entry-point */
#define EPT16BitEntry(a)        (((a).fType & EPT_BIT_MASK) == EPT_16BIT)
#define EPT32BitEntry(a)        (((a).fType & EPT_BIT_MASK) == EPT_32BIT)
#define EPTProc(a)              (((a).fType & ~(EPT_BIT_MASK | EPT_WRAPPED | EPT_NOT_REQ)) == EPT_PROC)
#define EPTProcHybrid(a)        (((a).fType & ~(EPT_BIT_MASK | EPT_WRAPPED | EPT_NOT_REQ)) == EPT_PROCH)
#define EPTVar(a)               (((a).fType & ~(EPT_BIT_MASK | EPT_NOT_REQ)) == EPT_VAR)

/* 32bit types */
#define EPT_PROC32              (EPT_PROC | EPT_32BIT)
#define EPT_PROCNR32            (EPT_PROC | EPT_32BIT | EPT_NOT_REQ)
#define EPT_VAR32               (EPT_VAR  | EPT_32BIT)
#define EPT_VARNR32             (EPT_VAR  | EPT_32BIT | EPT_NOT_REQ)
#define EPT32Proc(a)            (((a).fType & ~(EPT_WRAPPED)) == EPT_PROC32)
#define EPT32ProcNR(a)          (((a).fType & ~(EPT_WRAPPED)) == EPT_PROCNR32)
#define EPT32Var(a)             (((a).fType & ~(EPT_WRAPPED)) == EPT_VAR32)
#define EPT32VarNR(a)           (((a).fType & ~(EPT_WRAPPED)) == EPT_VARNR32)

/* 16bit types */
#define EPT_PROC16              (EPT_PROC  | EPT_16BIT)
#define EPT_PROCNR16            (EPT_PROC  | EPT_16BIT | EPT_NOT_REQ)
#define EPT_PROCH16             (EPT_PROCH | EPT_16BIT)  /* far proc in 16-bit calltab with a far jmp. */
#define EPT_PROCHNR16           (EPT_PROCH | EPT_16BIT | EPT_NOT_REQ)
#define EPT_VAR16               (EPT_VAR   | EPT_16BIT)
#define EPT_VARNR16             (EPT_VAR   | EPT_16BIT | EPT_NOT_REQ)
#define EPT16Proc(a)            (((a).fType & ~(EPT_WRAPPED)) == EPT_PROC16)
#define EPT16ProcNR(a)          (((a).fType & ~(EPT_WRAPPED)) == EPT_PROCNR16)
#define EPT16ProcH(a)           (((a).fType & ~(EPT_WRAPPED)) == EPT_PROCH16)
#define EPT16ProcHNR(a)         (((a).fType & ~(EPT_WRAPPED)) == EPT_PROCHNR16)
#define EPT16Var(a)             (((a).fType & ~(EPT_WRAPPED)) == EPT_VAR16)
#define EPT16VarNR(a)           (((a).fType & ~(EPT_WRAPPED)) == EPT_VARNR16)


/* Kernel type: SMP/UNI/W4 (flags matches KF_* in options.h)  */
#if 0
#define TYPE_UNI                0x00     /* Any UNI processor kernel except Warp 4 fp13 and above. */
#define TYPE_SMP                0x01     /* SMP Warp3 Adv. or Warp 4.5 SMP */
#define TYPE_W4                 0x02     /* Warp4 fp13 and above. */
#endif


/*
 * Calltab entry sizes.
 */
#define OVERLOAD16_ENTRY    0x18        /* This is intentionally 4 bytes larger than the one defined in calltaba.asm. */
#define OVERLOAD32_ENTRY    0x14
#define IMPORTH16_ENTRY     0x08
#define VARIMPORT_ENTRY     0x10


/*
 * Opcode.
 */
#define OPCODE_IGNORE   0               /* chOpcode value. */


/*
 * Fixup stuff.
 */
/* entry tab types*/
#define EXP_ABSOLUTE16  1
#define EXP_ABSOLUTE32  2
#define EXP_32          3
#define EXP_16          4
/* import tab types */
#define EXP_IMPORT_FIRST EXP_PROC32
#define EXP_PROC32      5
#define EXP_VAR32       6
#define EXP_VAR16       7
#define EXP_PROC16      8
#define EXP_ORGPROC32   9
#define EXP_ORGPROC16   10


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#pragma pack(1)
typedef struct _IMPORTKRNLSYM
{
    signed short int    iOrdinal;       /* The ordinal for this entry. (Count two ordinals for proc imports.) */
    unsigned char       fFound;         /* This is set when name is found */
    unsigned char       iObject;        /* Object number the name was found in */
    unsigned short int  cchName;        /* Length of the name (optmize search) (INPUT) */
    unsigned char       achName[MAX_LENGTH_NAME]; /* Name (INPUT) */
    unsigned char       achExtra[4];    /* Parameter extra. */
    unsigned long  int  offObject;      /* Offset into the object */
    unsigned long  int  ulAddress;      /* 32-bit flat address */
    unsigned short int  usSel;          /* Select of the object */
    unsigned char       cbProlog;       /* Size of the prolog needing to be exchanged */
    unsigned char       chOpcode;       /* The opcode of the function. 0 if not known. */
    unsigned char       fType;          /* Entry-Point Type Flags */
} IMPORTKRNLSYM, *PIMPORTKRNLSYM;
#pragma pack()


/*
 * Database of kernel symbols.
 */
#pragma pack(1)
typedef struct _KRNLDBENTRY
{
    unsigned short  usBuild;            /* Build number */
    unsigned short  fKernel;            /* Kernel flag (KF_* defines in options.h). */
    unsigned char   cObjects;           /* Count of objects */
    struct
    {
        unsigned char   iObject;        /* Object number.  */
        unsigned char   chOpcode;       /* The opcode of the function. 0 if not known. */
        unsigned long   offObject;      /* offset into object of the symbol. */
    } aSyms[NBR_OF_KRNLIMPORTS];

} KRNLDBENTRY, *PKRNLDBENTRY;
#pragma pack()



/*******************************************************************************
*   Global Variables                                                           *
*   NOTE! These are only available at init time!                               *
*******************************************************************************/
extern IMPORTKRNLSYM        aImportTab[NBR_OF_KRNLIMPORTS]; /* Defined in krnlImportTable.c */
extern const KRNLDBENTRY    aKrnlSymDB32[];                 /* Defined in symdb32.c */

#if defined(__IBMC__) || defined(__IBMCPP__)
    #pragma map( aImportTab , "_aImportTab"  )
#endif

#endif

