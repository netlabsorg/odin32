/* $Id: hll.h,v 1.3 2000-03-26 21:56:37 bird Exp $
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
 * HLL Directory entry types.
 */
#define HLL_DE_MODULES          0x0101  /* Filename */
#define HLL_DE_PUBLICS          0x0102  /* Public symbols */
#define HLL_DE_TYPES            0x0103  /* Types */
#define HLL_DE_SYMBOLS          0x0104  /* Symbols */
#define HLL_DE_LIBRARIES        0x0106  /* Libraries */
#define HLL_DE_SRCLINES         0x0105  /* Line numbers - (IBM C/2 1.1) */
#define HLL_DE_SRCLNSEG         0x0109  /* Line numbers - (MSC 6.00) */
#define HLL_DE_IBMSRC           0x010B  /* Line numbers - (IBM HLL) */


/*
 * HLL Module debug style
 */
#define HLL_MOD_STYLE           0x4C48  /* 'HL' */


/*
 * HLL Public symbol wide flag.
 */
#define HLL_PFS_WIDE            0x6e    /* Wide flag. */



/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/

/*
 * HLL Header
 */
typedef struct _HLLHdr
{
    unsigned char   achSignature[4];    /* LX Debug Info Signature, 'NB04' */
    unsigned long   offDirectory;       /* Offset to the HLL Directory.
                                         * (Relative to start of this header.) */
    unsigned long   ulReserved;         /* Unknown field. Seems to be 1. */
} HLLHDR, *PHLLHDR;


/*
 * HLL Directory Entry.
 */
typedef struct _HLLDirEntry
{
    unsigned short  usType;             /* Entry type. HLL_DE_* flag. */
    unsigned short  iMod;               /* Module number data applies to. */
    unsigned long   off;                /* Offset to data. This is based at
                                         * the start of the start of the debug
                                         * info. */
    unsigned long   cb;                 /* Size of data. */
} HLLDIRENTRY, *PHLLDIRENTRY;



/*
 * HLL Directory.
 */
typedef struct _HLLDirectory
{
    unsigned long   ulReserved;         /* Unknown. */
    unsigned long   cEntries;           /* Count of directory entires. */
    HLLDIRENTRY     aEntries[1];        /* Directory. */
} HLLDIR, *PHLLDIR;


/*
 * HLL Segment
 */
typedef struct _HLLSegInfo
{
    unsigned short  iObject;            /* LX Object number. */
    unsigned long   off;                /* Offset into the load image. */
    unsigned long   cb;                 /* Object length. */
} HLLSEGINFO, *PHLLSEGINFO;


/*
 * HLL Module (file)
 */
typedef struct _HLLModule
{
    HLLSEGINFO      SegInfo0;           /* Segment info entry 0. */
    unsigned short  overlay;            /* unused. */
    unsigned short  iLib;               /* Library number it came from. */
    unsigned char   cSegInfo;           /* Number of segment info entries. */
    unsigned char   pad;                /* 1 byte padding. */
    unsigned short  usDebugStyle;       /* Debug style -'HL' */
    unsigned char   chVerMinor;         /* HLL version - minor number. */
    unsigned char   chVerMajor;         /* HLL version - major number. */
    unsigned char   cchName;            /* Filename length. */
    unsigned char   achName[1];         /* Filename. (*) */
    /* HLLSEGINFO      aSegInfo[] */    /* Array of segment info, starting with entry 1. (Starts at achName[cchName]) */
} HLLMODULE, *PHLLMODULE;


/*
 * HLL Public Symbols
 */
typedef struct _HLLPublicSymbol
{
    unsigned long   off;                /* 32-bit offset (into the LX object) of the symbol location. */
    unsigned short  iObject;            /* LX Object number. */
    unsigned short  iType;              /* Symbol type index (into the type info data). */
    unsigned char   cchName;            /* Size of name. */
    unsigned char   achName[1];         /* Name (*) */
} HLLPUBLICSYM, *PHLLPUBLICSYM;





#pragma pack()

#endif
