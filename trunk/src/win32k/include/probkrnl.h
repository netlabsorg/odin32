/* $Id: probkrnl.h,v 1.11 2000-02-25 18:15:06 bird Exp $
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
#define NBR_OF_KRNLIMPORTS      17      /* When this is changed make sure to   */
                                        /* update the arrays in d32init.c and  */
                                        /* probkrnl32.c */
#define MAX_LENGTH_NAME         32      /* Max length for the function. */

/* Entry-Point Type flag */
#define EPT_PROC                0x00    /* procedure - overload procedure*/
#define EPT_PROCIMPORT          0x01    /* procedure 32bit - import only */
#define EPT_VARIMPORT           0x02    /* variable/non-procedure 32bit - not implemented yet */
#define EPT_32BIT               0x00    /* 16 bit entry-point  */
#define EPT_16BIT               0x80    /* 32 bit entry-point */
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
    unsigned char  chType;              /* R, H, A */
    unsigned char  fSMP;                /* TRUE / FALSE */
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
extern unsigned short int _usBuild;     /* 'ulBuild'         in PrbKrnl.c */
extern unsigned short int _usVerMajor;  /* 'usVerMajor'      in PrbKrnl.c */
extern unsigned short int _usVerMinor;  /* 'usVerMinor'      in PrbKrnl.c */
extern KRNLDBENTRY aKrnlSymDB[];        /* defined in symdb.c (for 16-bit usage) */
extern KRNLDBENTRY _aKrnlSymDB[];       /* defined in symdb.c (for 32-bit usage) */


#if defined(INCL_16) && defined(MAX_DISKDD_CMD) /* 16-bit only */
int ProbeKernel(PRPINITIN pReqPack);
#endif

#endif

