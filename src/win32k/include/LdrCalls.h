/* $Id: LdrCalls.h,v 1.3 2000-01-22 18:21:00 bird Exp $
 *
 * Prototypes for the loader overrided function.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _ldrCalls_h_
#define _ldrCalls_h_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define LDRCALL __stdcall


/**
 * _ldrClose
 */
extern ULONG LDRCALL ldrClose(  /* retd  0x04 */
    SFN p1                      /* ebp + 0x08 */
    );

ULONG LDRCALL myldrClose(SFN);


/**
 * _ldrOpen
 */
extern ULONG LDRCALL ldrOpen(   /* retd  0x0c */
    PSFN p1,                    /* ebp + 0x08 */
    PCHAR p2,                   /* ebp + 0x0c */
    ULONG p3                    /* ebp + 0x10 */
    );

ULONG LDRCALL myldrOpen(PSFN,PCHAR,ULONG);


/**
 * _ldrRead
 */
extern ULONG LDRCALL ldrRead(   /* retd  0x18 */
    SFN p1,                     /* ebp + 0x08 */
    ULONG p2,                   /* ebp + 0x0c */
    PVOID p3,                   /* ebp + 0x10 */
    ULONG p4,                   /* ebp + 0x14 */
    ULONG p5,                   /* ebp + 0x18 */
    PMTE p6                     /* ebp + 0x1c */
    );

ULONG LDRCALL myldrRead(
    SFN   hFile,
    ULONG ulOffset,
    PVOID pBuffer,
    ULONG ulFlags ,
    ULONG ulBytesToRead,
    PMTE  pMTE
    );



/**
 * _LDRQAppType
 */
extern ULONG LDRCALL LDRQAppType(   /* retd  0x08 */
    ULONG p1,                       /* ebp + 0x08 */
    ULONG p2                        /* ebp + 0x0c */
    );

ULONG LDRCALL myLDRQAppType(ULONG,ULONG);


/**
 * ldrEnum32bitRelRecs
 * @param  pMTE           Pointer to MTE for this module.
 * @param  iObject        Object index. 0-based!
 * @param  iPageTabl      Page index. 0-based!
 * @param  pvPage         Pointer to page buffer.
 * @param  ulPageAddress  Note! Page is not present.
 * @param  pPTDA
 *
 */
extern ULONG LDRCALL ldrEnum32bitRelRecs( /* retd 0x20 */
    PMTE pMTE,                      /* ebp + 0x08 */
    ULONG iObject,                  /* ebp + 0x0c */
    ULONG iPageTable,               /* ebp + 0x10 */
    PVOID pvPage,                   /* ebp + 0x14 */
    ULONG ulPageAddress,            /* ebp + 0x18 */
    PVOID pvPTDA                    /* ebp + 0x1c */
    );

ULONG LDRCALL myldrEnum32bitRelRecs(PMTE, ULONG, ULONG, PVOID, ULONG, PVOID);


#if 0
/****************************************/
/* _LDRLoadExe                          */
/****************************************/
extern int LDRCALL _LDRLoadExe(     /* retd  0x08 */
        PCHAR pExeName,             /* ebp + 0x08 */
        PULONG param2               /* ebp + 0x0c */
    );

int LDRCALL myLDRLoadExe(PCHAR,PULONG);



/****************************************/
/* _ldrGetResource                      */
/****************************************/
extern int LDRCALL _ldrGetResource( /* retd 0x14 - 20d */
        USHORT idType,              /* ebp + 0x08 */
        ULONG  idName,              /* ebp + 0x0c */
        USHORT hMod,                /* ebp + 0x10 */
        PPVOID ppRes,               /* ebp + 0x14 */
        ULONG ulP5                  /* ebp + 0x18 */
    );

int LDRCALL myldrGetResource(USHORT,ULONG,USHORT,PPVOID,ULONG);



/****************************************/
/* _ldrOpenNewExe                       */
/****************************************/
extern int LDRCALL _ldrOpenNewExe(  /* retd 0x10 - 16d */
        PCHAR  pszName,             /* ebp + 0x08 */
        USHORT lenName,             /* ebp + 0x0c */
        ULONG  ulP3,                /* ebp + 0x10 */
        ULONG  ulP4                 /* ebp + 0x14 */
    );

int LDRCALL myldrOpenNewExe(PCHAR,USHORT,ULONG,ULONG);



/****************************************/
/* _ldrCreateMte                        */
/****************************************/
extern int LDRCALL  _ldrCreateMte(  /* retd  0x08 */
        ULONG p1,                   /* ebp + 0x08 */
        ULONG p2                    /* ebp + 0x0c */
    );

int LDRCALL myldrCreateMte(ULONG,ULONG);



/****************************************/
/* _ldrGetMte                           */
/****************************************/
extern int LDRCALL  _ldrGetMte(     /* retd 0x14 - 20d */
        ULONG  p1,                  /* ebp + 0x08 */
        ULONG  p2,                  /* ebp + 0x0c */
        ULONG  p3,                  /* ebp + 0x10 */
        ULONG  p4,                  /* ebp + 0x14 */
        ULONG  p5                   /* ebp + 0x18 */
    );

int LDRCALL myldrGetMte(ULONG,ULONG,ULONG,ULONG,ULONG);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

