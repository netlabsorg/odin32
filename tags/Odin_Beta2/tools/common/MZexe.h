/* $Id: MZexe.h,v 1.1 2002-02-24 02:47:22 bird Exp $
 *
 * MZ DEFINITIONS AND DECLARATIONS.
 * --------------------------------
 *
 * Copyright Robert J. Amstadt 1993
 * Copyright IBM Corp 1984-1992
 * Copyright IBM Corp 1987-1992
 * Copyright Microsoft Corp 1984-1987
 * Copyright knut st. osmundsen 2001 (I've messed them up ;-)
 *
 */

#ifndef _MZexe_h_
#define _MZexe_h_


/*
 * The EXE.h way of declaring this struct.
 */
struct exe
{
    unsigned short  eid;                /* contains EXEID, below */
    unsigned short  elast;              /* # of bytes in last page */
    unsigned short  epagsiz;            /* # of pages in whole file */
    unsigned short  erelcnt;            /* # of relocation entrys */
    unsigned short  ehdrsiz;            /* size of header, in paragraphs */
    unsigned short  eminfre;            /* min # of free paragraphs needed */
    unsigned short  emaxfre;            /* max # of free paragraphs needed */
    unsigned short  eiSS;               /* initial SS value */
    unsigned short  eiSP;               /* initial SP value */
    unsigned short  enegsum;            /* negative sum of entire file */
    unsigned short  eiIP;               /* initial IP value */
    unsigned short  eiCS;               /* initial CS value */
    unsigned short  ereloff;            /* offset in file of relocation table */
    unsigned short  eovlnum;            /* number of the overlay */

/*
 *  the following fields may not be present.
 *          ereloff = 28            not present
 *                  = 30            exe.ever present and valid
 *                  = 32            exe.ever field contains garbage
 *          ereloff > 32            exe.ever present and valid
 *                                          = 0 if "don't know"
 */
    unsigned short  ever;               /* version # of producing linker */
    unsigned short  dumy;               /* unused */

/*
 *  the following fields may not be present - if the exe.ereloff
 *  value encompasses the fields then they are present and valid.
 */
    unsigned short  ebb;                /* behavior bits */
    unsigned short  dumy2[7];           /* must be 0 until defined */
};



/*
 * The NEWEXE.h way of declaring this header.
 */
#define EMAGIC          0x5A4D          /* Old magic number */
#define ENEWEXE         sizeof(struct exe_hdr) /* Value of E_LFARLC for new .EXEs */
#define ENEWHDR         0x003C          /* Offset in old hdr. of ptr. to new */
#define ERESWDS         0x0010          /* No. of reserved words (OLD) */
#define ERES1WDS        0x0004          /* No. of reserved words in e_res */
#define ERES2WDS        0x000A          /* No. of reserved words in e_res2 */
#define ECP             0x0004          /* Offset in struct of E_CP */
#define ECBLP           0x0002          /* Offset in struct of E_CBLP */
#define EMINALLOC       0x000A          /* Offset in struct of E_MINALLOC */
struct exe_hdr                          /* DOS 1, 2, 3 .EXE header */
{
    unsigned short      e_magic;        /* Magic number */
    unsigned short      e_cblp;         /* Bytes on last page of file */
    unsigned short      e_cp;           /* Pages in file */
    unsigned short      e_crlc;         /* Relocations */
    unsigned short      e_cparhdr;      /* Size of header in paragraphs */
    unsigned short      e_minalloc;     /* Minimum extra paragraphs needed */
    unsigned short      e_maxalloc;     /* Maximum extra paragraphs needed */
    unsigned short      e_ss;           /* Initial (relative) SS value */
    unsigned short      e_sp;           /* Initial SP value */
    unsigned short      e_csum;         /* Checksum */
    unsigned short      e_ip;           /* Initial IP value */
    unsigned short      e_cs;           /* Initial (relative) CS value */
    unsigned short      e_lfarlc;       /* File address of relocation table */
    unsigned short      e_ovno;         /* Overlay number */
    unsigned short      e_res[ERES1WDS];/* Reserved words */
    unsigned short      e_oemid;        /* OEM identifier (for e_oeminfo) */
    unsigned short      e_oeminfo;      /* OEM information; e_oemid specific */
    unsigned short      e_res2[ERES2WDS];/* Reserved words */
    long                e_lfanew;       /* File address of new exe header */
};



/*
 * The NT way of declaring this header.
 */
typedef struct
{
    unsigned short  e_magic;            /* MZ Header signature */
    unsigned short  e_cblp;             /* Bytes on last page of file */
    unsigned short  e_cp;               /* Pages in file */
    unsigned short  e_crlc;             /* Relocations */
    unsigned short  e_cparhdr;          /* Size of header in paragraphs */
    unsigned short  e_minalloc;         /* Minimum extra paragraphs needed */
    unsigned short  e_maxalloc;         /* Maximum extra paragraphs needed */
    unsigned short  e_ss;               /* Initial (relative) SS value */
    unsigned short  e_sp;               /* Initial SP value */
    unsigned short  e_csum;             /* Checksum */
    unsigned short  e_ip;               /* Initial IP value */
    unsigned short  e_cs;               /* Initial (relative) CS value */
    unsigned short  e_lfarlc;           /* File address of relocation table */
    unsigned short  e_ovno;             /* Overlay number */
    unsigned short  e_res[4];           /* Reserved words */
    unsigned short  e_oemid;            /* OEM identifier (for e_oeminfo) */
    unsigned short  e_oeminfo;          /* OEM information; e_oemid specific */
    unsigned short  e_res2[10];         /* Reserved words */
    unsigned long   e_lfanew;           /* Offset to extended header */
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

#define IMAGE_DOS_SIGNATURE 0x5A4D /* MZ */



#endif
