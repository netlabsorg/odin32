/* $Id: exceptstackdump.h,v 1.1 2000-05-02 20:53:12 sandervl Exp $ */
#ifndef __EXCEPTSTACKDUMP_H__
#define __EXCEPTSTACKDUMP_H__

/*
 *      This file incorporates code from the following:
 *      -- Marc Fiammante, John Currier, Kim Rasmussen,
 *         Anthony Cruise (EXCEPT3.ZIP package for a generic
 *         exception handling DLL, available at Hobbes).
 *
 *      This file Copyright (C) 1992-99 Ulrich M”ller,
 *                                      Kim Rasmussen,
 *                                      Marc Fiammante,
 *                                      John Currier,
 *                                      Anthony Cruise.
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, in version 2 as it comes in the COPYING
 *      file of the XFolder main distribution.
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 */

#ifndef DEBUG_HEADER_INCLUDED
    #define DEBUG_HEADER_INCLUDED

    /********************************************************************
     *                                                                  *
     *   SYM file declarations                                          *
     *                                                                  *
     ********************************************************************/

    // Pointer means offset from beginning of file or beginning of struct
    #pragma pack(1)
    typedef struct {
        unsigned short int ppNextMap;     // paragraph pointer to next map
        unsigned char      bFlags;        // symbol types
        unsigned char      bReserved1;    // reserved
        unsigned short int pSegEntry;     // segment entry point value
        unsigned short int cConsts;       // count of constants in map
        unsigned short int pConstDef;     // pointer to constant chain
        unsigned short int cSegs;         // count of segments in map
        unsigned short int ppSegDef;      // paragraph pointer to first segment
        unsigned char      cbMaxSym;      // maximum symbol-name length
        unsigned char      cbModName;     // length of module name
        char               achModName[1]; // cbModName Bytes of module-name member
    } MAPDEF;

    typedef struct {
        unsigned short int ppNextMap;     // always zero
        unsigned char      release;       // release number (minor version number)
        unsigned char      version;       // major version number
    } LAST_MAPDEF;

    typedef struct {
        unsigned short int ppNextSeg;     // paragraph pointer to next segment
        unsigned short int cSymbols;      // count of symbols in list
        unsigned short int pSymDef;       // offset of symbol chain
        unsigned short int wReserved1;    // reserved
        unsigned short int wReserved2;    // reserved
        unsigned short int wReserved3;    // reserved
        unsigned short int wReserved4;    // reserved
        unsigned char      bFlags;        // symbol types; bit 0 signals 32-bit (*UM)
        unsigned char      bReserved1;    // reserved
        unsigned short int ppLineDef;     // offset of line number record
        unsigned char      bReserved2;    // reserved
        unsigned char      bReserved3;    // reserved
        unsigned char      cbSegName;     // length of segment name
        char               achSegName[1]; /* cbSegName Bytes of segment-name member*/
    } SEGDEF;

    typedef struct {
        unsigned short int wSymVal;       // symbol address or constant
        unsigned char      cbSymName;     // length of symbol name
        char               achSymName[1]; // cbSymName Bytes of symbol-name member
    } SYMDEF16;

    typedef struct {
        unsigned       int wSymVal;       // symbol address or constant
        unsigned char      cbSymName;     // length of symbol name
        char               achSymName[1]; // cbSymName Bytes of symbol-name member
    } SYMDEF32;

    typedef struct {
        unsigned short int ppNextLine;    // ptr to next linedef (0 if last)
        unsigned short int wReserved1;    // reserved
        unsigned short int pLines;        // pointer to line numbers
        unsigned short int cLines;        // reserved
        unsigned char      cbFileName;    // length of filename
        char               achFileName[1];// cbFileName Bytes of filename
    } LINEDEF;

    typedef struct {
        unsigned short int wCodeOffset;   // executable offset
        unsigned short int dwFileOffset;  // source offset
    } LINEINF;

    #define SEGDEFOFFSET(MapDef)     (MapDef.ppSegDef*16)
    #define NEXTSEGDEFOFFSET(SegDef)  (SegDef.ppNextSeg*16)

    #define ASYMPTROFFSET(SegDefOffset,Segdef) (SegDefOffset+SegDef.pSymDef)
    #define SYMDEFOFFSET(SegDefOffset,SegDef,n) (ASYMPTROFFSET(SegDefOffset,SegDef)+(n)*(sizeof(unsigned short int)))

    #define ACONSTPTROFFSET(MapDef) (MapDef.ppConstDef)
    #define CONSTDEFOFFSET(MapDef,n) ((MapDef.ppConstDef)+(n)*(sizeof(unsigned short int)))

    #define LINEDEFOFFSET(SegDef) (SegDef.ppLineDef*16)
    #define NEXTLINEDEFOFFSET(LineDef) (LineDef.ppNextLine*16)
    #define LINESOFFSET(LinedefOffset,LineDef) ((LinedefOffset)+LineDef.pLines)

    /********************************************************************
     *                                                                  *
     *   Object Module Format (OMF) declarations                        *
     *                                                                  *
     ********************************************************************/

    struct exehdr_rec {
       BYTE     signature[2];              // Must be "MZ"
       USHORT   image_len;                 // Image Length
       USHORT   pages;                     // Pages
       USHORT   reloc_items;               // Relocation table items
       USHORT   min_paragraphs;            // Mininum 16-bytes paragraphs
       USHORT   max_paragraphs;            // Maximum 16-bytes paragraphs
       USHORT   stack_pos;                 // Stack position
       USHORT   offset_in_sp;              // Offset in SP
       USHORT   checksum;                  // Checksum
       USHORT   offset_in_ip;              // Offset in IP
       USHORT   code_pos;                  // Code segment pos.
       USHORT   reloc_item_pos;            // Position of first relocation item
       USHORT   overlay_number;            // Overlay number
       BYTE     unused[8];                 // Unused bytes
       USHORT   oem_id;                    // OEM Identifier
       BYTE     oem_info[24];              // OEM Info
       ULONG    lexe_offset;               // Offset to linear header
    };

    struct lexehdr_rec {
       BYTE     signature[2];              // Must be "LX"
       BYTE     b_ord;                     // Byte ordering
       BYTE     w_ord;                     // Word ordering
       ULONG    format_level;              // Format level
       USHORT   cpu_type;                  // CPU Type
       USHORT   os_type;                   // Operating system
       ULONG    module_version;            // Module version
       ULONG    mod_flags;                 // Module flags
       ULONG    mod_pages;                 // Module pages
       ULONG    EIP_object;                // EIP Object no.
       ULONG    EIP;                       // EIP Value
       ULONG    ESP_object;                // ESP Object no
       ULONG    ESP;                       // ESP Value
       ULONG    page_size;                 // Page size
       ULONG    page_ofs_shift;            // Page offset shift
       ULONG    fixup_sect_size;           // Fixup section size
       ULONG    fixup_sect_checksum;       // Fixup section checksum
       ULONG    loader_sect_size;          // Loader section size
       ULONG    loader_sect_checksum;      // Loader section checksum
       ULONG    obj_table_ofs;             // Object table offset
       ULONG    obj_count;                 // Object count
       ULONG    obj_page_tab_ofs;          // Object page table offset
       ULONG    obj_iter_page_ofs;         // Object iteration pages offset
       ULONG    res_tab_ofs;               // Resource table offset
       ULONG    res_table_entries;         // Resource table entries
       ULONG    res_name_tab_ofs;          // Resident name table offset;
       ULONG    ent_tab_ofs;               // Entry table offset
       ULONG    mod_dir_ofs;               // Module directives offset
       ULONG    mod_dir_count;             // Number of module directives
       ULONG    fixup_page_tab_ofs;        // Fixup page table offset
       ULONG    fixup_rec_tab_ofs;         // Fixup record table offset
       ULONG    imp_tab_ofs;               // Import module table offset
       ULONG    imp_mod_entries;           // Import module entries
       ULONG    imp_proc_tab_ofs;          // Import proc table offset
       ULONG    per_page_check_ofs;        // Per page checksum offset
       ULONG    data_page_offset;          // Data pages offset
       ULONG    preload_page_count;        // Preload pages count
       ULONG    nonres_tab_ofs;            // Nonresident name table offset
       ULONG    nonres_tab_len;            // Nonresident name table len
       ULONG    nonres_tab_check;          // Nonresident tables checksum
       ULONG    auto_ds_objectno;          // Auto DS object number
       ULONG    debug_info_ofs;            // Debug info offset
       ULONG    debug_info_len;            // Debug info length
       ULONG    inst_preload_count;        // Instance preload count
       ULONG    inst_demand_count;         // Instance demand count
       ULONG    heapsize;                  // Heap size
       ULONG    stacksize;                 // Stack size
    };

    struct debug_head_rec {
       BYTE signature[3];                  // Debug signature
       BYTE type;                          // Debug info type
    };

    struct dir_inf_rec {
       USHORT   dirstruct_size;            // Size of directory structure
       USHORT   number_of_entries;         // Number of dnt_rec's in the array
       USHORT   unknown;                   // Unknown data
       // Followed by an array of dnt_rec structures
    };

    struct dnt_rec {
       USHORT   subsect_type;              // sst Subsection type
       USHORT   mod_index;                 // Module index (1-based)
       ULONG    offset;                    // Offset of start of section
       ULONG    size;                      // Size of section
    };

    // Modules subsection
    struct modules_rec {
       USHORT   code_seg_base;             // Code segment base
       ULONG    code_seg_offset;           // Code segment offset
       ULONG    code_seg_len;              // Code segment length
       USHORT   overlay_no;                // Overlay number
       USHORT   lib_idx;                   // Index into library section or 0
       BYTE     segments;                  // Number of segments
       BYTE     reserved;
       BYTE     debug_style[2];            // "HL" for HLL, "CV" or 0 for CodeView
       BYTE     debug_version[2];          // 00 01 or 00 03 for HLL, 00 00 for CV
       BYTE     name_len;                  // Length of name (which follows)
    };

    // Publics subsection
    struct publics_rec {
       ULONG    offset;                    // Offset
       USHORT   segment;                   // Segment
       USHORT   type;                      // Type index
       BYTE     name_len;                  // Length of name (wich follows)
    };

    #if 0
    // Linenumbers header
    struct linhead_rec {
       BYTE     id;                        // 0x95 for flat mem, 32 bit progs
       USHORT   length;                    // Record length
       USHORT   base_group;                // Base group
       USHORT   base_segment;              // Base segment
    };
    #endif

    // First linenumber record
    struct linfirst_rec {
       USHORT   lineno;                    // Line number (0)
       BYTE     entry_type;                // Entry type
       BYTE     reserved;                  // Reserved
       USHORT   entries_count;             // Number of table entries
       USHORT   segment_no;                // Segment number
       ULONG    filename_tabsize;          // File names table size
    };


    // Source line numbers
    struct linsource_rec {
       USHORT   source_line;               // Source file line number
       USHORT   source_idx;                // Source file index
       ULONG    offset;                    // Offset into segment
    };


    // Listing statement numbers
    struct linlist_rec {
       ULONG    list_line;                 // Listing file linenumber
       ULONG    statement;                 // Listing file statement number
       ULONG    offset;                    // Offset into segment
    };


    // Source and Listing statement numbers
    struct linsourcelist_rec {
       USHORT   source_line;               // Source file line number
       USHORT   source_idx;                // Source file index
       ULONG    list_line;                 // Listing file linenumber
       ULONG    statement;                 // Listing file statement number
       ULONG    offset;                    // Offset into segment
    };


    // Path table
    struct pathtab_rec {
       ULONG    offset;                    // Offset into segment
       USHORT   path_code;                 // Path code
       USHORT   source_idx;                // Source file index
    };


    // File names table
    struct filenam_rec {
       ULONG    first_char;                // First displayable char in list file
       ULONG    disp_chars;                // Number of displayable chars in list line
       ULONG    filecount;                 // Number of source/listing files
    };


    // Symbol types
    #define SYM_BEGIN          0x00        // Begin block
    #define SYM_PROC           0x01        // Function
    #define SYM_END            0x02        // End block of function
    #define SYM_AUTO           0x04        // Auto variable
    #define SYM_STATIC         0x05        // Static variable
    #define SYM_LABEL          0x0B        // Label
    #define SYM_WITH           0x0C        // With start symbol (not used)
    #define SYM_REG            0x0D        // Register variable
    #define SYM_CONST          0x0E        // Constant
    #define SYM_ENTRY          0x0F        // Secondary entry (not in C)
    #define SYM_SKIP           0x10        // For incremental linking (not used)
    #define SYM_CHANGESEG      0x11        // Change segment (#pragma alloc_text)
    #define SYM_TYPEDEF        0x12        // Typedef variable
    #define SYM_PUBLIC         0x13        // Public reference
    #define SYM_MEMBER         0x14        // Member of minor or major structure
    #define SYM_BASED          0x15        // Based variable
    #define SYM_TAG            0x16        // Tag in struct, union, enum ...
    #define SYM_TABLE          0x17        // Table (used in RPG - not C)
    #define SYM_MAP            0x18        // Map variable (extern in C)
    #define SYM_CLASS          0x19        // Class symbol (C++)
    #define SYM_MEMFUNC        0x1A        // Member function
    #define SYM_AUTOSCOPE      0x1B        // Scoped auto for C++ (not used)
    #define SYM_STATICSCOPE    0x1C        // scoped static for C++ (not used)
    #define SYM_CPPPROC        0x1D        // C++ Proc
    #define SYM_CPPSTAT        0x1E        // C++ Static var
    #define SYM_COMP           0x40        // Compiler information


    // Symbolic begin record
    struct symbegin_rec {
       ULONG    offset;                    // Segment offset
       ULONG    length;                    // Length of block
       BYTE     name_len;                  // Length of block name
       // Block name follows
    };


    // Symbolic auto var record
    struct symauto_rec {
       ULONG    stack_offset;              // Stack offset
       USHORT   type_idx;                  // Type index
       BYTE     name_len;                  // Length of name
       // Var name follows
    };


    // Symbolic procedure record
    struct symproc_rec {
       ULONG    offset;                    // Segment offset
       USHORT   type_idx;                  // Type index
       ULONG    length;                    // Length of procedure
       USHORT   pro_len;                   // Length of prologue
       ULONG    pro_bodylen;               // Length of prologue + body
       USHORT   class_type;                // Class type
       BYTE     near_far;                  // Near or far
       BYTE     name_len;                  // Length of name
       // Function name follows
    };


    // Symbolic static var record
    struct symstatic_rec {
       ULONG    offset;                    // Segment offset
       USHORT   segaddr;                   // Segment address
       USHORT   type_idx;                  // Type index
       BYTE     name_len;                  // Length of name
       // Var name follows
    };


    // Symbolic label var record
    struct symlabel_rec {
       ULONG    offset;                    // Segment offset
       BYTE     near_far;                  // Near or far
       BYTE     name_len;                  // Length of name
       // Var name follows
    };

    // Symbolic register var record
    struct symreg_rec {
       USHORT   type_idx;                  // Type index
       BYTE     reg_no;                    // Register number
       BYTE     name_len;                  // Length of name
       // Var name follows
    };

    // Symbolic change-segment record
    struct symseg_rec {
       USHORT   seg_no;                    // Segment number
    };

    // Symbolic typedef record
    struct symtypedef_rec {
       USHORT   type_idx;                  // Type index
       BYTE     name_len;                  // Length of name
       // Name follows
    };

    // Symbolic public record
    struct sympublic_rec {
       ULONG    offset;                    // Segment offset
       USHORT   segaddr;                   // Segment address
       USHORT   type_idx;                  // Type index
       BYTE     name_len;                  // Length of name
       // Name follows
    };

    // Symbolic member record
    struct symmember_rec {
       ULONG    offset;                    // Offset to subrecord
       BYTE     name_len;                  // Length of name
       // Name follows
    };

    // Symbolic based record
    struct symbased_rec {
       ULONG    offset;                    // Offset to subrecord
       USHORT   type_idx;                  // Type index
       BYTE     name_len;                  // Length of name
       // Name follows
    };

    // Symbolic tag record
    struct symtag_rec {
       USHORT   type_idx;                  // Type index
       BYTE     name_len;                  // Length of name
       // Name follows
    };


    // Symbolic table record
    struct symtable_rec {
       ULONG    offset;                    // Segment offset
       USHORT   segaddr;                   // Segment address
       USHORT   type_idx;                  // Type index
       ULONG    idx_ofs;                   // Index offset to subrecord
       BYTE     name_len;                  // Length of name
       // Name follows
    };


    // Type record
    struct type_rec {
       USHORT   length;                    // Length of sub-record
       BYTE     type;                      // Sub-record type
       BYTE     type_qual;                 // Type qualifier
    };



    // Types
    #define TYPE_CLASS         0x40        // Class
    #define TYPE_BASECLASS     0x41        // Base class
    #define TYPE_FRIEND        0x42        // Friend
    #define TYPE_CLASSDEF      0x43        // Class definition
    #define TYPE_MEMBERFUNC    0x45        // Member function
    #define TYPE_CLASSMEMBER   0x46        // Class member
    #define TYPE_REF           0x48        // Reference
    #define TYPE_MEMBERPTR     0x49        // Member pointer
    #define TYPE_SCALARS       0x51        // Scalars
    #define TYPE_SET           0x52        // Set
    #define TYPE_ENTRY         0x53        // Entry
    #define TYPE_FUNCTION      0x54        // Function
    #define TYPE_AREA          0x55        // Area
    #define TYPE_LOGICAL       0x56        // Logical
    #define TYPE_STACK         0x57        // Stack
    #define TYPE_MACRO         0x59        // Macro
    #define TYPE_BITSTRING     0x5C        // Bit string
    #define TYPE_USERDEF       0x5D        // User defined
    #define TYPE_CHARSTR       0x60        // Character string
    #define TYPE_PICTURE       0x61        // Picture
    #define TYPE_GRAPHIC       0x62        // Graphic
    #define TYPE_FORMATLAB     0x65        // Format label
    #define TYPE_FILE          0x67        // File
    #define TYPE_SUBRANGE      0x6F        // Subrange
    #define TYPE_CODELABEL     0x72        // Code label
    #define TYPE_PROCEDURE     0x75        // Procedure
    #define TYPE_ARRAY         0x78        // Array
    #define TYPE_STRUCT        0x79        // Structure / Union / Record
    #define TYPE_POINTER       0x7A        // Pointer
    #define TYPE_ENUM          0x7B        // Enum
    #define TYPE_LIST          0x7F        // List


    // Type userdef
    struct type_userdefrec {
       BYTE     FID_index;                 // Field ID
       USHORT   type_index;                // Type index
       BYTE     FID_string;                // String ID
       BYTE     name_len;                  // Length of name which follows
    };


    // Type function
    struct type_funcrec {
       USHORT   params;
       USHORT   max_params;
       BYTE     FID_index;                 // Field ID
       USHORT   type_index;                // Type index of return value
       BYTE     FID_index1;                // String ID
       USHORT   typelist_index;            // Index of list of params
    };


    // Type struct
    struct type_structrec {
       ULONG    size;                      // Size of structure
       USHORT   field_count;               // Number of fields in structure
       BYTE     FID_index;                 // Field ID
       USHORT   type_list_idx;             // Index to type list
       BYTE     FID_index1;                // Field ID
       USHORT   type_name_idx;             // Index to names / offsets
       BYTE     dont_know;                 // Haven't a clue, but it seems to be needed
       BYTE     name_len;                  // Length of structure name which follows
    };

    // Type list, type qualifier 1: contains types for structures
    // This record is repeated for the number of items in the structure definition
    struct type_list1 {
       BYTE     FID_index;                 // Field identifier for index
       USHORT   type_index;                // Type index.
    };

    // Type list, type qualifier 2: contains names and offsets for structure items
    // This record is repeated for the number of items in the structure definition
    struct type_list2 {
       BYTE     FID_string;                // String identifier
       BYTE     name_len;                  // Length of name which follows
    };

    // Type list, subrecord to the above, contains offset of variable in the structure
    struct type_list2_1 {
       BYTE     FID_span;                  // Defines what type of variable follows
       union {
          BYTE   b_len;
          USHORT s_len;
          ULONG  l_len;
       } u;
    };

    // Type pointer
    struct type_pointerrec {
       BYTE     FID_index;                 // Index identifier
       USHORT   type_index;                // Type index
       BYTE     FID_string;                // String identifier
       BYTE     name_len;                  // Length of name which follows
    };
    #pragma pack()

    /*
     * dbgPrintStack:
     *      this takes stack data from the TIB and
     *      context record data structures and tries
     *      to analyse what the different stack frames
     *      correspond to in the source files.
     *      This analysis is either tried upon debug
     *      code, if present, or otherwise from a
     *      .SYM file with the same filestem.
     *
     *      New with V0.84.
     *
     *      This is based on the EXCEPT3.ZIP package
     *      for the generic EXCEPTQ.DLL exception handler.
     *      Appently (W) 1995 or later John Currier.
     *      See the copyright notices on top of this file.
     */


     void dbgPrintStack(PEXCEPTIONREPORTRECORD       pERepRec,
                        PEXCEPTIONREGISTRATIONRECORD pERegRec,
                        PCONTEXTRECORD               pCtxRec,
                        PVOID                        p);

    /*
     * DosQueryModFromEIP:
     *      we need to prototype this undocumented OS/2 API.
     *      Apparently, it doesn't have to be imported, but
     *      it's not in the header files.
     *      This is available on OS/2 2.1 and above.
     *      This function may be used to get a program module
     *      name, segment number and offset within the segment
     *      if you have an absolute address within the calling
     *      program. Very useful for exception handlers.
     */


extern "C"
{
    APIRET APIENTRY DosQueryModFromEIP(HMODULE *phMod,   // out: trapping module
                                       ULONG *pulObjNum, // out: object/segment number
                                       ULONG ulBuffLen,  // in:  sizeof(*pszBuff)
                                       CHAR *pszBuff,    // out: module name
                                       ULONG *pulOffset, // out: offset in module
                                       ULONG ulAddress); // in:  address to be analyzed

   APIRET APIENTRY DosQueryModFromEIP (PULONG pulModule,
                                       PULONG pulObject,
                                       ULONG  ulBufferLength,
                                       PSZ    pszBuffer,
                                       PULONG pulOffset,
                                       ULONG  ulEIP);

    typedef ULONG  *_Seg16 PULONG16;
    APIRET16 APIENTRY16 DOS16SIZESEG(USHORT Seg, PULONG16 Size);
    typedef         APIRET16(APIENTRY16 _PFN16) ();
    ULONG APIENTRY  DosSelToFlat(ULONG);

    APIRET16 APIENTRY16 DOSQPROCSTATUS(ULONG * _Seg16 pBuf, USHORT cbBuf);

    #define CONVERT(fp,QSsel) MAKEP((QSsel),OFFSETOF(fp))
}
#endif

#endif //__EXCEPTSTACKDUMP_H__
