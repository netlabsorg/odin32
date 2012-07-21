/* $Id: neexe.h,v 1.3 2003-04-02 11:02:35 sandervl Exp $ */

/*
 * Copyright  Robert J. Amstadt, 1993
 */
#ifndef __WINE_NEEXE_H
#define __WINE_NEEXE_H

#ifdef __WIN32OS2__
#ifndef _OS2WIN_H
#include <win32type.h>
#endif
#else
#include "windef.h"
#endif

/*
 * Old MZ header for DOS programs.
 * We check only the magic and the e_lfanew offset to the new executable
 * header.
 */
#include "pshpack2.h"
typedef struct
{
    WORD  e_magic;      /* 00: MZ Header signature */
    WORD  e_cblp;       /* 02: Bytes on last page of file */
    WORD  e_cp;         /* 04: Pages in file */
    WORD  e_crlc;       /* 06: Relocations */
    WORD  e_cparhdr;    /* 08: Size of header in paragraphs */
    WORD  e_minalloc;   /* 0a: Minimum extra paragraphs needed */
    WORD  e_maxalloc;   /* 0c: Maximum extra paragraphs needed */
    WORD  e_ss;         /* 0e: Initial (relative) SS value */
    WORD  e_sp;         /* 10: Initial SP value */
    WORD  e_csum;       /* 12: Checksum */
    WORD  e_ip;         /* 14: Initial IP value */
    WORD  e_cs;         /* 16: Initial (relative) CS value */
    WORD  e_lfarlc;     /* 18: File address of relocation table */
    WORD  e_ovno;       /* 1a: Overlay number */
    WORD  e_res[4];     /* 1c: Reserved words */
    WORD  e_oemid;      /* 24: OEM identifier (for e_oeminfo) */
    WORD  e_oeminfo;    /* 26: OEM information; e_oemid specific */
    WORD  e_res2[10];   /* 28: Reserved words */
    DWORD e_lfanew;     /* 3c: Offset to extended header */
} IMAGE_DOS_HEADER,*PIMAGE_DOS_HEADER;
#include "poppack.h"

#define	IMAGE_DOS_SIGNATURE	0x5A4D		/* MZ */
#define	IMAGE_OS2_SIGNATURE	0x454E		/* NE */
#define	IMAGE_OS2_SIGNATURE_LE	0x454C		/* LE */
#define	IMAGE_OS2_SIGNATURE_LX  0x584C          /* LX */
#define	IMAGE_VXD_SIGNATURE	0x454C		/* LE */
#define	IMAGE_NT_SIGNATURE	0x00004550	/* PE00 */

/*
 * This is the Windows executable (NE) header.
 * the name IMAGE_OS2_HEADER is misleading, but in the SDK this way.
 */
#include "pshpack2.h"
typedef struct
{
    WORD  ne_magic;             /* 00 NE signature 'NE' */
    BYTE  ne_ver;               /* 02 Linker version number */
    BYTE  ne_rev;               /* 03 Linker revision number */
    WORD  ne_enttab;            /* 04 Offset to entry table relative to NE */
    WORD  ne_cbenttab;          /* 06 Length of entry table in bytes */
    LONG  ne_crc;               /* 08 Checksum */
    WORD  ne_flags;             /* 0c Flags about segments in this file */
    WORD  ne_autodata;          /* 0e Automatic data segment number */
    WORD  ne_heap;              /* 10 Initial size of local heap */
    WORD  ne_stack;             /* 12 Initial size of stack */
    DWORD ne_csip;              /* 14 Initial CS:IP */
    DWORD ne_sssp;              /* 18 Initial SS:SP */
    WORD  ne_cseg;              /* 1c # of entries in segment table */
    WORD  ne_cmod;              /* 1e # of entries in module reference tab. */
    WORD  ne_cbnrestab;         /* 20 Length of nonresident-name table     */
    WORD  ne_segtab;            /* 22 Offset to segment table */
    WORD  ne_rsrctab;           /* 24 Offset to resource table */
    WORD  ne_restab;            /* 26 Offset to resident-name table */
    WORD  ne_modtab;            /* 28 Offset to module reference table */
    WORD  ne_imptab;            /* 2a Offset to imported name table */
    DWORD ne_nrestab;           /* 2c Offset to nonresident-name table */
    WORD  ne_cmovent;           /* 30 # of movable entry points */
    WORD  ne_align;             /* 32 Logical sector alignment shift count */
    WORD  ne_cres;              /* 34 # of resource segments */
    BYTE  ne_exetyp;            /* 36 Flags indicating target OS */
    BYTE  ne_flagsothers;       /* 37 Additional information flags */
    WORD  ne_pretthunks;        /* 38 Offset to return thunks */
    WORD  ne_psegrefbytes;      /* 3a Offset to segment ref. bytes */
    WORD  ne_swaparea;          /* 3c Reserved by Microsoft */
    WORD  ne_expver;            /* 3e Expected Windows version number */
} IMAGE_OS2_HEADER, *PIMAGE_OS2_HEADER;
#include "poppack.h"
/*
 * NE Header FORMAT FLAGS
 */
#define NE_FFLAGS_SINGLEDATA	0x0001
#define NE_FFLAGS_MULTIPLEDATA	0x0002
#define NE_FFLAGS_WIN32         0x0010
#define NE_FFLAGS_BUILTIN       0x0020  /* Wine built-in module */
#define NE_FFLAGS_FRAMEBUF	0x0100  /* OS/2 fullscreen app */
#define NE_FFLAGS_CONSOLE	0x0200  /* OS/2 console app */
#define NE_FFLAGS_GUI		0x0300	/* right, (NE_FFLAGS_FRAMEBUF | NE_FFLAGS_CONSOLE) */
#define NE_FFLAGS_SELFLOAD	0x0800
#define NE_FFLAGS_LINKERROR	0x2000
#define NE_FFLAGS_CALLWEP       0x4000
#define NE_FFLAGS_LIBMODULE	0x8000

/*
 * NE Header OPERATING SYSTEM
 */
#define NE_OSFLAGS_UNKNOWN	0x01
#define NE_OSFLAGS_WINDOWS	0x04

/*
 * NE Header ADDITIONAL FLAGS
 */
#define NE_AFLAGS_WIN2_PROTMODE	0x02
#define NE_AFLAGS_WIN2_PROFONTS	0x04
#define NE_AFLAGS_FASTLOAD	0x08

/*
 * Segment table entry
 */
struct ne_segment_table_entry_s
{
    WORD seg_data_offset;	/* Sector offset of segment data	*/
    WORD seg_data_length;	/* Length of segment data		*/
    WORD seg_flags;		/* Flags associated with this segment	*/
    WORD min_alloc;		/* Minimum allocation size for this	*/
};

/*
 * Segment Flags
 */
#define NE_SEGFLAGS_DATA	0x0001
#define NE_SEGFLAGS_ALLOCATED	0x0002
#define NE_SEGFLAGS_LOADED	0x0004
#define NE_SEGFLAGS_ITERATED	0x0008
#define NE_SEGFLAGS_MOVEABLE	0x0010
#define NE_SEGFLAGS_SHAREABLE	0x0020
#define NE_SEGFLAGS_PRELOAD	0x0040
#define NE_SEGFLAGS_EXECUTEONLY	0x0080
#define NE_SEGFLAGS_READONLY	0x0080
#define NE_SEGFLAGS_RELOC_DATA	0x0100
#define NE_SEGFLAGS_SELFLOAD	0x0800
#define NE_SEGFLAGS_DISCARDABLE	0x1000

/*
 * Relocation table entry
 */
struct relocation_entry_s
{
    BYTE address_type;	/* Relocation address type		*/
    BYTE relocation_type;	/* Relocation type			*/
    WORD offset;		/* Offset in segment to fixup		*/
    WORD target1;		/* Target specification			*/
    WORD target2;		/* Target specification			*/
};

/*
 * Relocation address types
 */
#define NE_RADDR_LOWBYTE	0
#define NE_RADDR_SELECTOR	2
#define NE_RADDR_POINTER32	3
#define NE_RADDR_OFFSET16	5
#define NE_RADDR_POINTER48	11
#define NE_RADDR_OFFSET32	13

/*
 * Relocation types
 */
#define NE_RELTYPE_INTERNAL	0
#define NE_RELTYPE_ORDINAL	1
#define NE_RELTYPE_NAME		2
#define NE_RELTYPE_OSFIXUP	3
#define NE_RELFLAG_ADDITIVE	4

/*
 * Resource table structures.
 */
struct resource_nameinfo_s
{
    unsigned short offset;
    unsigned short length;
    unsigned short flags;
    unsigned short id;
    HANDLE16 handle;
    unsigned short usage;
};

struct resource_typeinfo_s
{
    unsigned short type_id;	/* Type identifier */
    unsigned short count;	/* Number of resources of this type */
    FARPROC16	   resloader;	/* SetResourceHandler() */
    /*
     * Name info array.
     */
};

#define NE_RSCTYPE_ACCELERATOR		0x8009
#define NE_RSCTYPE_BITMAP		0x8002
#define NE_RSCTYPE_CURSOR		0x8001
#define NE_RSCTYPE_DIALOG		0x8005
#define NE_RSCTYPE_FONT			0x8008
#define NE_RSCTYPE_FONTDIR		0x8007
#define NE_RSCTYPE_GROUP_CURSOR		0x800c
#define NE_RSCTYPE_GROUP_ICON		0x800e
#define NE_RSCTYPE_ICON			0x8003
#define NE_RSCTYPE_MENU			0x8004
#define NE_RSCTYPE_RCDATA		0x800a
#define NE_RSCTYPE_STRING		0x8006

#endif  /* __WINE_NEEXE_H */
