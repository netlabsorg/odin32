/* $Id: probkrnl.h,v 1.13.4.1 2000-07-16 22:43:33 bird Exp $
 *
 * Include file for ProbKrnl.
 *
 * Copyright (c) 1998-2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _ProbKrnl_h_
#define _ProbKrnl_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define NBR_OF_KRNLIMPORTS      30      /* When this is changed make sure to   */
                                        /* update the arrays in d32init.c and  */
                                        /* probkrnl.c */
#define MAX_LENGTH_NAME         32      /* Max length for the function. */

/* Entry-Point Type flag */
#define EPT_PROC                0x00    /* procedure - overload procedure*/
#define EPT_PROCIMPORT          0x01    /* procedure 32bit - import only */
#define EPT_VARIMPORT           0x02    /* variable/non-procedure 32bit - not implemented yet */
#define EPT_32BIT               0x00    /* 32 bit entry-point  */
#define EPT_16BIT               0x80    /* 16 bit entry-point */
#define EPT_BIT_MASK            0x80    /* Mask bit entry-point */
#define EPT16BitEntry(a)    (((a).fType & EPT_BIT_MASK) == EPT_16BIT)
#define EPT32BitEntry(a)    (((a).fType & EPT_BIT_MASK) == EPT_32BIT)

/* 32bit types */
#define EPT_PROC32              (EPT_PROC | EPT_32BIT)
#define EPT_PROCIMPORT32        (EPT_PROCIMPORT | EPT_32BIT)
#define EPT_VARIMPORT32         (EPT_VARIMPORT | EPT_32BIT)

/* 16bit types */
#define EPT_PROC16              (EPT_PROC | EPT_16BIT)        /* no implemented yet! */
#define EPT_PROCIMPORT16        (EPT_PROCIMPORT | EPT_16BIT)  /* far proc in calltab with a far jmp. */
#define EPT_VARIMPORT16         (EPT_VARIMPORT | EPT_16BIT)


/* Kernel type: SMP/UNI/W4 (flags matches KF_* in options.h)  */
#if 0
#define TYPE_UNI                0x00     /* Any UNI processor kernel except Warp 4 fp13 and above. */
#define TYPE_SMP                0x01     /* SMP Warp3 Adv. or Warp 4.5 SMP */
#define TYPE_W4                 0x02     /* Warp4 fp13 and above. */
#endif

/*
 * Probkrnl error codes. (They all begin with ERROR_PROB_.)
 *
 * NOTE! ERROR_PROB_KRNL_*  <  ERROR_PROB_SYM_*  <  ERROR_PROB_SYMDB_*
 */
#define ERROR_PROB_BASE                             10000
#define ERROR_PROB_KRNL_FIRST                       ERROR_PROB_KRNL_OPEN_FAILED
#define ERROR_PROB_KRNL_LAST                        ERROR_PROB_KRNL_OTE_READ
#define ERROR_PROB_KRNL_OPEN_FAILED                 (ERROR_PROB_BASE + 0)
#define ERROR_PROB_KRNL_SEEK_SIZE                   (ERROR_PROB_BASE + 2)
#define ERROR_PROB_KRNL_SEEK_FIRST                  (ERROR_PROB_BASE + 3)
#define ERROR_PROB_KRNL_READ_FIRST                  (ERROR_PROB_BASE + 4)
#define ERROR_PROB_KRNL_READ_NEXT                   (ERROR_PROB_BASE + 5)
#define ERROR_PROB_KRNL_TAG_NOT_FOUND               (ERROR_PROB_BASE + 6)
#define ERROR_PROB_KRNL_INV_SIGANTURE               (ERROR_PROB_BASE + 7)
#define ERROR_PROB_KRNL_INV_BUILD_NBR               (ERROR_PROB_BASE + 8)
#define ERROR_PROB_KRNL_BUILD_VERSION               (ERROR_PROB_BASE + 9)
#define ERROR_PROB_KRNL_MZ_SEEK                     (ERROR_PROB_BASE + 10)
#define ERROR_PROB_KRNL_MZ_READ                     (ERROR_PROB_BASE + 11)
#define ERROR_PROB_KRNL_NEOFF_INVALID               (ERROR_PROB_BASE + 12)
#define ERROR_PROB_KRNL_NEOFF_SEEK                  (ERROR_PROB_BASE + 13)
#define ERROR_PROB_KRNL_LX_READ                     (ERROR_PROB_BASE + 14)
#define ERROR_PROB_KRNL_LX_SIGNATURE                (ERROR_PROB_BASE + 15)
#define ERROR_PROB_KRNL_OBJECT_CNT                  (ERROR_PROB_BASE + 16)
#define ERROR_PROB_KRNL_OBJECT_CNR_10               (ERROR_PROB_BASE + 17)
#define ERROR_PROB_KRNL_OTE_SEEK                    (ERROR_PROB_BASE + 18)
#define ERROR_PROB_KRNL_OTE_READ                    (ERROR_PROB_BASE + 19)
#define ERROR_PROB_KRNL_OTE_SIZE_MIS                (ERROR_PROB_BASE + 20)

#define ERROR_PROB_SYM_FIRST                        ERROR_PROB_SYM_FILE_NOT_FOUND
#define ERROR_PROB_SYM_LAST                         ERROR_PROB_SYM_IMPORTS_NOTFOUND
#define ERROR_PROB_SYM_FILE_NOT_FOUND               (ERROR_PROB_BASE + 30)
#define ERROR_PROB_SYM_READERROR                    (ERROR_PROB_BASE + 31)
#define ERROR_PROB_SYM_INVALID_MOD_NAME             (ERROR_PROB_BASE + 32)
#define ERROR_PROB_SYM_SEGS_NE_OBJS                 (ERROR_PROB_BASE + 33)
#define ERROR_PROB_SYM_SEG_DEF_SEEK                 (ERROR_PROB_BASE + 34)
#define ERROR_PROB_SYM_SEG_DEF_READ                 (ERROR_PROB_BASE + 35)
/* - reserved for future errors - *
#define ERROR_PROB_SYM_                             (ERROR_PROB_BASE + 36)
#define ERROR_PROB_SYM_                             (ERROR_PROB_BASE + 37)
#define ERROR_PROB_SYM_                             (ERROR_PROB_BASE + 38) */
#define ERROR_PROB_SYM_IMPORTS_NOTFOUND             (ERROR_PROB_BASE + 39)

#define ERROR_PROB_SYMDB_KRNL_NOT_FOUND             (ERROR_PROB_BASE + 40)



/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#pragma pack(1)
typedef struct tagIMPORTKRNLSYM
{
   unsigned char       fFound;          /* This is set when name is found */
   unsigned char       iObject;         /* Object number the name was found in*/
   unsigned short int  cchName;         /* Length of the name (optmize search) (INPUT) */
   unsigned char       achName[MAX_LENGTH_NAME]; /* Name (INPUT) */
   unsigned long  int  offObject;       /* Offset into the object */
   unsigned long  int  ulAddress;       /* 32-bit flat address */
   unsigned short int  usSel;           /* Select of the object */
   unsigned char       cbProlog;        /* Size of the prolog needing to be exchanged */
   unsigned char       fType;           /* Entry-Point Type Flags */
} IMPORTKRNLSYM;
#pragma pack()


/*
 * Database of kernel symbols.
 */
#pragma pack(1)
typedef struct
{
    unsigned short usBuild;             /* Build number */
    unsigned short fKernel;             /* Kernel flag (KF_* defines in options.h). */
    unsigned char  cObjects;            /* Count of objects */
    struct
    {
        unsigned char iObject;          /* Object number.  */
        unsigned long offObject;        /* offset into object of the symbol. */
    } aSyms[NBR_OF_KRNLIMPORTS];

} KRNLDBENTRY, *PKRNLDBENTRY;
#pragma pack()



/*******************************************************************************
*   Global Variables                                                           *
*   NOTE! These are only available at init time!                               *
*******************************************************************************/
extern IMPORTKRNLSYM _aImportTab[NBR_OF_KRNLIMPORTS]; /* 'aImportTab' in PrbKrnl.c */
extern KRNLDBENTRY   aKrnlSymDB[];      /* defined in symdb.c (for 16-bit usage) */
extern KRNLDBENTRY   _aKrnlSymDB[];     /* defined in symdb.c (for 32-bit usage) */

#if defined(INCL_16) && defined(MAX_DISKDD_CMD) /* 16-bit only */
int ProbeKernel(PRPINITIN pReqPack);
#endif

#endif

