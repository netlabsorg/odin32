/* $Id: probkrnl.h,v 1.14 2000-09-02 21:08:04 bird Exp $
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
#define NBR_OF_KRNLIMPORTS      42      /* When this is changed make sure to   */
                                        /* update the arrays in d32init.c and  */
                                        /* probkrnl.c, make test faker in      */
                                        /* test.h and the appropriate fake.c   */
                                        /* fakea.asm.                          */
#define MAX_LENGTH_NAME         32      /* Max length for the function. */

/* Entry-Point Type flag */
#define EPT_PROC                0x00    /* procedure - overload procedure*/
#define EPT_PROCIMPORT          0x01    /* procedure 32bit - import only */
#define EPT_VARIMPORT           0x02    /* variable/non-procedure 32bit */
#define EPT_NOT_REQ             0x04    /* Not required flag. */
#define EPT_32BIT               0x00    /* 32 bit entry-point  */
#define EPT_16BIT               0x80    /* 16 bit entry-point */
#define EPT_BIT_MASK            0x80    /* Mask bit entry-point */
#define EPT16BitEntry(a)    (((a).fType & EPT_BIT_MASK) == EPT_16BIT)
#define EPT32BitEntry(a)    (((a).fType & EPT_BIT_MASK) == EPT_32BIT)

/* 32bit types */
#define EPT_PROC32              (EPT_PROC | EPT_32BIT)
#define EPT_PROCIMPORT32        (EPT_PROCIMPORT | EPT_32BIT)
#define EPT_PROCIMPORTNR32      (EPT_PROCIMPORT | EPT_32BIT | EPT_NOT_REQ)
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
extern IMPORTKRNLSYM DATA16_GLOBAL  aImportTab[NBR_OF_KRNLIMPORTS]; /* Defined in ProbKrnl.c */
extern char          DATA16_GLOBAL  szUsrSym[50];                   /* Defined in ProbKrnl.c */
extern KRNLDBENTRY   DATA16_INIT    aKrnlSymDB[];                   /* Defined in symdb.c (for 16-bit usage) */

#if defined(__IBMC__) || defined(__IBMCPP__)
    #pragma map( aImportTab , "_aImportTab" )
    #pragma map( szUsrSym   , "_szUsrSym"   )
    #pragma map( aKrnlSymDB , "_aKrnlSymDB" )
#endif

#if defined(INCL_16) && defined(MAX_DISKDD_CMD) /* 16-bit only */
int ProbeKernel(PRPINITIN pReqPack);

#if 0 /*ndef CODE16_INIT*/
#pragma alloc_text(CODE16_INIT, ProbeKernel)
#endif

#endif

#endif

