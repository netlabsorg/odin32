/* $Id: NEexe.h,v 1.1 2002-02-24 02:47:23 bird Exp $
 *
 * NE DEFINITIONS AND DECLARATIONS.
 * --------------------------------
 *
 * Copyright Robert J. Amstadt, 1993
 * Copyright knut st. osmundsen (2001) (I've messed them up ;-)
 *
 */

#ifndef _NEexe_h_
#define _NEexe_h_

#define IMAGE_OS2_SIGNATURE     0x454E /* NE */

/*
 * This is the Windows executable (NE) header.
 * the name IMAGE_OS2_HEADER is misleading, but in the SDK this way.
 */
typedef struct
{
    unsigned short  ne_magic;             /* 00 NE signature 'NE' */
    unsigned char   ne_ver;               /* 02 Linker version number */
    unsigned char   ne_rev;               /* 03 Linker revision number */
    unsigned short  ne_enttab;            /* 04 Offset to entry table relative to NE */
    unsigned short  ne_cbenttab;          /* 06 Length of entry table in bytes */
    unsigned long   ne_crc;               /* 08 Checksum */
    unsigned short  ne_flags;             /* 0c Flags about segments in this file */
    unsigned short  ne_autodata;          /* 0e Automatic data segment number */
    unsigned short  ne_heap;              /* 10 Initial size of local heap */
    unsigned short  ne_stack;             /* 12 Initial size of stack */
    unsigned long   ne_csip;              /* 14 Initial CS:IP */
    unsigned long   ne_sssp;              /* 18 Initial SS:SP */
    unsigned short  ne_cseg;              /* 1c # of entries in segment table */
    unsigned short  ne_cmod;              /* 1e # of entries in module reference tab. */
    unsigned short  ne_cbnrestab;         /* 20 Length of nonresident-name table     */
    unsigned short  ne_segtab;            /* 22 Offset to segment table */
    unsigned short  ne_rsrctab;           /* 24 Offset to resource table */
    unsigned short  ne_restab;            /* 26 Offset to resident-name table */
    unsigned short  ne_modtab;            /* 28 Offset to module reference table */
    unsigned short  ne_imptab;            /* 2a Offset to imported name table */
    unsigned long   ne_nrestab;           /* 2c Offset to nonresident-name table */
    unsigned short  ne_cmovent;           /* 30 # of movable entry points */
    unsigned short  ne_align;             /* 32 Logical sector alignment shift count */
    unsigned short  ne_cres;              /* 34 # of resource segments */
    unsigned char   ne_exetyp;            /* 36 Flags indicating target OS */
    unsigned char   ne_flagsothers;       /* 37 Additional information flags */
    unsigned short  fastload_offset;      /* 38 Offset to fast load area (should be ne_pretthunks)*/
    unsigned short  fastload_length;      /* 3a Length of fast load area (should be ne_psegrefbytes) */
    unsigned short  ne_swaparea;          /* 3c Reserved by Microsoft */
    unsigned short  ne_expver;            /* 3e Expected Windows version number */
} IMAGE_OS2_HEADER, *PIMAGE_OS2_HEADER;

/*
 * NE Header FORMAT FLAGS
 */
#define NE_FFLAGS_SINGLEDATA    0x0001
#define NE_FFLAGS_MULTIPLEDATA  0x0002
#define NE_FFLAGS_WIN32         0x0010
#define NE_FFLAGS_BUILTIN       0x0020  /* Wine built-in module */
#define NE_FFLAGS_FRAMEBUF      0x0100  /* OS/2 fullscreen app */
#define NE_FFLAGS_CONSOLE       0x0200  /* OS/2 console app */
#define NE_FFLAGS_GUI           0x0300  /* right, (NE_FFLAGS_FRAMEBUF | NE_FFLAGS_CONSOLE) */
#define NE_FFLAGS_SELFLOAD      0x0800
#define NE_FFLAGS_LINKERROR     0x2000
#define NE_FFLAGS_CALLWEP       0x4000
#define NE_FFLAGS_LIBMODULE     0x8000

/*
 * NE Header OPERATING SYSTEM
 */
#define NE_OSFLAGS_UNKNOWN      0x01
#define NE_OSFLAGS_WINDOWS      0x04

/*
 * NE Header ADDITIONAL FLAGS
 */
#define NE_AFLAGS_WIN2_PROTMODE 0x02
#define NE_AFLAGS_WIN2_PROFONTS 0x04
#define NE_AFLAGS_FASTLOAD      0x08

/*
 * Segment table entry
 */
struct ne_segment_table_entry_s
{
    unsigned short  seg_data_offset;   /* Sector offset of segment data    */
    unsigned short  seg_data_length;   /* Length of segment data       */
    unsigned short  seg_flags;     /* Flags associated with this segment   */
    unsigned short  min_alloc;     /* Minimum allocation size for this */
};

/*
 * Segment Flags
 */
#define NE_SEGFLAGS_DATA        0x0001
#define NE_SEGFLAGS_ALLOCATED   0x0002
#define NE_SEGFLAGS_LOADED      0x0004
#define NE_SEGFLAGS_ITERATED    0x0008
#define NE_SEGFLAGS_MOVEABLE    0x0010
#define NE_SEGFLAGS_SHAREABLE   0x0020
#define NE_SEGFLAGS_PRELOAD     0x0040
#define NE_SEGFLAGS_EXECUTEONLY 0x0080
#define NE_SEGFLAGS_READONLY    0x0080
#define NE_SEGFLAGS_RELOC_DATA  0x0100
#define NE_SEGFLAGS_SELFLOAD    0x0800
#define NE_SEGFLAGS_DISCARDABLE 0x1000

/*
 * Relocation table entry
 */
struct relocation_entry_s
{
    unsigned char   address_type;  /* Relocation address type      */
    unsigned char   relocation_type;   /* Relocation type          */
    unsigned short  offset;        /* Offset in segment to fixup       */
    unsigned short  target1;       /* Target specification         */
    unsigned short  target2;       /* Target specification         */
};

/*
 * Relocation address types
 */
#define NE_RADDR_LOWBYTE        0
#define NE_RADDR_SELECTOR       2
#define NE_RADDR_POINTER32      3
#define NE_RADDR_OFFSET16       5
#define NE_RADDR_POINTER48      11
#define NE_RADDR_OFFSET32       13

/*
 * Relocation types
 */
#define NE_RELTYPE_INTERNAL     0
#define NE_RELTYPE_ORDINAL      1
#define NE_RELTYPE_NAME         2
#define NE_RELTYPE_OSFIXUP      3
#define NE_RELFLAG_ADDITIVE     4

/*
 * Resource table structures.
 */
struct resource_nameinfo_s
{
    unsigned short  offset;
    unsigned short  length;
    unsigned short  flags;
    unsigned short  id;
    unsigned short  handle;
    unsigned short  usage;
};

struct resource_typeinfo_s
{
    unsigned short  type_id;    /* Type identifier */
    unsigned short  count;      /* Number of resources of this type */
    unsigned long   resloader;  /* SetResourceHandler() */
    /*
     * Name info array.
     */
};

#define NE_RSCTYPE_ACCELERATOR  0x8009
#define NE_RSCTYPE_BITMAP       0x8002
#define NE_RSCTYPE_CURSOR       0x8001
#define NE_RSCTYPE_DIALOG       0x8005
#define NE_RSCTYPE_FONT         0x8008
#define NE_RSCTYPE_FONTDIR      0x8007
#define NE_RSCTYPE_GROUP_CURSOR 0x800c
#define NE_RSCTYPE_GROUP_ICON   0x800e
#define NE_RSCTYPE_ICON         0x8003
#define NE_RSCTYPE_MENU         0x8004
#define NE_RSCTYPE_RCDATA       0x800a
#define NE_RSCTYPE_STRING       0x8006

#endif
