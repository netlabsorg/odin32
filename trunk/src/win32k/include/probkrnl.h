/* $Id: probkrnl.h,v 1.22 2001-07-10 05:22:15 bird Exp $
 *
 * Include file for ProbKrnl.
 *
 * Copyright (c) 1998-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _ProbKrnl_h_
#define _ProbKrnl_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define NBR_OF_KRNLIMPORTS      91      /* When this is changed make sure to   */
                                        /* update the aImportTab in probkrnl.c */
                                        /* and make test faker in test.h and   */
                                        /* the appropriate fake.c or fakea.asm.*/
#define MAX_LENGTH_NAME         32      /* Max length for the function. */

/* Entry-Point Type flag */
#define EPT_PROC                0x00    /* procedure - overload procedure*/
#define EPT_PROCIMPORT          0x01    /* procedure - import only */
#define EPT_PROCIMPORTH         0x02    /* procedure hybrid - import only */
#define EPT_VARIMPORT           0x04    /* variable/non-procedure 32bit */
#define EPT_NOT_REQ             0x08    /* Not required flag. */
#define EPTNotReq(a)            (((a).fType & (EPT_NOT_REQ)) == EPT_NOT_REQ)
#define EPT_WRAPPED             0x40    /* Wrapped due - differs between builds */
#define EPTWrapped(a)           (((a).fType & (EPT_WRAPPED)) == EPT_WRAPPED)
#define EPT_32BIT               0x00    /* 32 bit entry-point  */
#define EPT_16BIT               0x80    /* 16 bit entry-point */
#define EPT_BIT_MASK            0x80    /* Mask bit entry-point */
#define EPT16BitEntry(a)        (((a).fType & EPT_BIT_MASK) == EPT_16BIT)
#define EPT32BitEntry(a)        (((a).fType & EPT_BIT_MASK) == EPT_32BIT)
#define EPTProcImportHybrid(a)  (((a).fType & ~(EPT_BIT_MASK | EPT_WRAPPED | EPT_NOT_REQ)) == EPT_PROCIMPORTH)

/* 32bit types */
#define EPT_PROC32              (EPT_PROC | EPT_32BIT)
#define EPT_PROCIMPORT32        (EPT_PROCIMPORT | EPT_32BIT)
#define EPT_PROCIMPORTNR32      (EPT_PROCIMPORT | EPT_32BIT | EPT_NOT_REQ)
#define EPT_VARIMPORT32         (EPT_VARIMPORT | EPT_32BIT)
#define EPT_VARIMPORTNR32       (EPT_VARIMPORT | EPT_32BIT | EPT_NOT_REQ)
#define EPT32Proc(a)            (((a).fType & ~(EPT_WRAPPED)) == EPT_PROC32)
#define EPT32ProcImport(a)      (((a).fType & ~(EPT_WRAPPED)) == EPT_PROCIMPORT32)
#define EPT32ProcImportNR(a)    (((a).fType & ~(EPT_WRAPPED)) == EPT_PROCIMPORTNR32)
#define EPT32VarImport(a)       (((a).fType & ~(EPT_WRAPPED)) == EPT_VARIMPORT32)
#define EPT32VarImportNR(a)     (((a).fType & ~(EPT_WRAPPED)) == EPT_VARIMPORTNR32)

/* 16bit types */
#define EPT_PROC16              (EPT_PROC | EPT_16BIT)
#define EPT_PROCIMPORT16        (EPT_PROCIMPORT | EPT_16BIT)  /* far proc in calltab with a far jmp. */
#define EPT_PROCIMPORTNR16      (EPT_PROCIMPORT | EPT_16BIT | EPT_NOT_REQ)
#define EPT_PROCIMPORTH16       (EPT_PROCIMPORTH | EPT_16BIT)  /* far proc in 16-bit calltab with a far jmp. */
#define EPT_PROCIMPORTHNR16     (EPT_PROCIMPORTH | EPT_16BIT | EPT_NOT_REQ)
#define EPT_VARIMPORT16         (EPT_VARIMPORT | EPT_16BIT)
#define EPT_VARIMPORTNR16       (EPT_VARIMPORT | EPT_16BIT | EPT_NOT_REQ)
#define EPT16Proc(a)            (((a).fType & ~(EPT_WRAPPED)) == EPT_PROC16)
#define EPT16ProcImport(a)      (((a).fType & ~(EPT_WRAPPED)) == EPT_PROCIMPORT16)
#define EPT16ProcImportNR(a)    (((a).fType & ~(EPT_WRAPPED)) == EPT_PROCIMPORTNR16)
#define EPT16ProcImportH(a)     (((a).fType & ~(EPT_WRAPPED)) == EPT_PROCIMPORTH16)
#define EPT16ProcImportHNR(a)   (((a).fType & ~(EPT_WRAPPED)) == EPT_PROCIMPORTHNR16)
#define EPT16VarImport(a)       (((a).fType & ~(EPT_WRAPPED)) == EPT_VARIMPORT16)
#define EPT16VarImportNR(a)     (((a).fType & ~(EPT_WRAPPED)) == EPT_VARIMPORTNR16)


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
   unsigned char       achExtra[4];     /* Parameter extra. */
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
extern char          DATA16_GLOBAL  szSymbolFile[60];               /* Defined in ProbKrnl.c */
extern KRNLDBENTRY   DATA16_INIT    aKrnlSymDB[];                   /* Defined in symdb.c (for 16-bit usage) */

#if defined(__IBMC__) || defined(__IBMCPP__)
    #pragma map( aImportTab , "_aImportTab"  )
    #pragma map( szSymbolFile,"_szSymbolFile")
    #pragma map( aKrnlSymDB , "_aKrnlSymDB"  )
#endif

/*
 * 16-bit init time functions.
 */
#if defined(INCL_16) && defined(MAX_DISKDD_CMD) /* 16-bit only */
int ProbeKernel(PRPINITIN pReqPack);
const char *    GetErrorMsg(short sErr);
#endif

#endif

