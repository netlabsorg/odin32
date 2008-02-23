/* $Id: elf.h,v 1.6 2000-12-11 06:53:51 bird Exp $
 *
 * ELF stuff.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _elf_h_
#define _elf_h_

//#pragma pack(1)

/*
 * Basic ELF types.
 */
typedef unsigned long   Elf32_Addr;   /* Unsigned program address. */
typedef unsigned short  Elf32_Half;   /* Unsigned medium integer.  */
typedef signed long     Elf32_Off;    /* Unsigned file offset.     */
typedef signed long     Elf32_Sword;  /* Signed large interger.    */
typedef unsigned long   Elf32_Word;   /* Unsigned large integer.   */
/*                      unsigned char    Unsigned small integer.   */



/*
 * ELF Header
 */
#define EI_INDENT         0x10

typedef struct                          /* 0x34 */
{
    unsigned char   e_ident[EI_INDENT]; /* 0x00  ELF Indentification */
    Elf32_Half      e_type;             /* 0x10  Object file type */
    Elf32_Half      e_machine;          /* 0x12  Machine type */
    Elf32_Word      e_version;          /* 0x14  ELF format version */
    Elf32_Addr      e_entry;            /* 0x18  Entry point address */
    Elf32_Off       e_phoff;            /* 0x1c  File offset of the program header table */
    Elf32_Off       e_shoff;            /* 0x20  File offset of the section header table */
    Elf32_Word      e_flags;            /* 0x24  Processor specific flags */
    Elf32_Half      e_ehsize;           /* 0x28  Size of the Elf Header (this structure) */
    Elf32_Half      e_phentsize;        /* 0x2a  Size of one entry in the program header table */
    Elf32_Half      e_phnum;            /* 0x2c  Number of entries in the program header table */
    Elf32_Half      e_shentsize;        /* 0x2e  Size of one entry in the section header table */
    Elf32_Half      e_shnum;            /* 0x30  Number of entries in the section header table */
    Elf32_Half      e_shstrndx;         /* 0x32  Section header table index of the string table */
} Elf32_Ehdr;

/* e_idnet - ELF Identification */
#define EI_MAG0         0               /* File identificator */
#define EI_MAG1         1               /* File identificator */
#define EI_MAG2         2               /* File identificator */
#define EI_MAG3         3               /* File identificator */
#define EI_CLASS        4               /* File class */
#define EI_DATA         5               /* Data encoding */
#define EI_VERSION      6               /* File version */
#define EI_PAD          7               /* Start of padding bytes */
    /* EI_MAG[0-4] */
#define ELFMAG0         0x7f
#define ELFMAG1         'E'
#define ELFMAG2         'L'
#define ELFMAG3         'F'
#define ELFMAGICLSB     ( 0x7f        | ('E' <<  8) | ('L' << 16) | ('F' << 24))
#define ELFMAGICMSB     ((0x7f << 24) | ('E' << 16) | ('L' <<  8) |  'F'       )
    /* EI_CLASS */
#define ELFCLASSNONE    0               /* Invalid class */
#define ELFCLASS32      1               /* 32-bit objects */
#define ELFCLASS64      2               /* 64-bit objects */
#define ELFCLASSNUM     3               /* ? */
    /* EI_DATA */
#define ELFDATANONE     0               /* Invalid data encoding */
#define ELFDATA2LSB     1               /* Little endian encoding */
#define ELFDATA2MSB     2               /* Big endian encoding */

/* e_type contents */
#define ET_NONE         0x0000          /* No file type */
#define ET_REL          0x0001          /* Relocatable file */
#define ET_EXEC         0x0002          /* Executable file */
#define ET_DYN          0x0003          /* Shared object file */
#define ET_CORE         0x0004          /* Core file */
#define ET_LOPROC       0xff00          /* Processor-specific - currect value? */
#define ET_HIPROC       0xffff          /* Processor-specific - currect value? */

/* e_machine contents */
#define EM_NONE         0               /* No machine */
#define EM_M32          1               /* AT&T WE 32100 */
#define EM_SPARC        2               /* SPARC */
#define EM_386          3               /* Intel 80386 */
#define EM_68K          4               /* Motorola 680000 */
#define EM_88K          5               /* Motorola 880000 */
#define EM_486          6               /* Intel 80486 - disused? */
#define EM_860          7               /* Intel 80860 */
#define EM_MIPS         8               /* MIPS RS3000 (big endian...) */
#define EM_MIPS_RS4_BE  10              /* MIPS RS4000 (big endian) */
#define EM_SPARC64      11              /* SPARC v9 (not official) 64-bit */
#define EM_PARISC       15              /* HPPA */
#define EM_SPARC32PLUS  18              /* SUN's "v8plus" */
#define EM_PPC          20              /* PowerPC */
#define EM_ALPHA        0x9026          /* Digital Alpha - NB! Interim number */

/* e_version contents */
#define EV_NONE         0               /* Invalid version */
#define EV_CURRENT      1               /* Current version */
#define EV_NUM          2               /* ? */

/* e_flags - Machine Information */
#define EF_386_NONE     0               /* Intel 80386 has no flags defined */
#define EF_486_NONE     0               /* Intel 80486 has no flags defined */


/*
 * ELF Sections
 */
typedef struct                          /* 0x28 */
{
    Elf32_Word      sh_name;            /* 0x00  Section name. Index into the section header string table section. */
    Elf32_Word      sh_type;            /* 0x04  Section type. Categorizes the contents and semantics. */
    Elf32_Word      sh_flags;           /* 0x08  Section flags. */
    Elf32_Addr      sh_addr;            /* 0x0c  Address to where in memory the section should reside.  */
    Elf32_Off       sh_offset;          /* 0x10  Offset into the file of the section's data. */
    Elf32_Word      sh_size;            /* 0x14  Section's size in bytes. */
    Elf32_Word      sh_link;            /* 0x18  Section header table index link. */
    Elf32_Word      sh_info;            /* 0x1c  Extra information. */
    Elf32_Word      sh_addralign;       /* 0x20  Section alignment. */
    Elf32_Word      sh_entsize;         /* 0x24  Entry size of some evtentual fixed-sized table entries. */
} Elf32_Shdr;

/* section indexes (section references) */
#define SHN_UNDEF       0               /* Undefined/missing/irrelevant/meaningless section reference. */
#define SHN_LORESERVE   0xff00          /* Lower bound of the reserved indexes. */
#define SHN_LOPROC      0xff00          /* Processor-specific semantics lower bound. */
#define SHN_HIPROC      0xff1f          /* Processor-specific semantics upper bound. */
#define SHN_ABS         0xfff1          /* Absolute values for the corresponding reference. */
#define SHN_COMMON      0xfff2          /* Symbols defined relative to this section are common symbols. */
#define SHN_HIRESERVE   0xffff          /* Upper bound of the reserved indexes. */

/* sh_type */
#define SHT_NULL        0               /* Inactive section header. All other members have undefined contents. */
#define SHT_PROGBITS    1               /* Program defined information. */
#define SHT_SYMTAB      2               /* Symboltable. */
#define SHT_STRTAB      3               /* Stringtable. */
#define SHT_RELA        4               /* Relocations with explicit addends. */
#define SHT_HASH        5               /* Symbol has table. */
#define SHT_DYNAMIC     6               /* Dynamic linking information. */
#define SHT_NOTE        7               /* Information tat marks the file in some way. */
#define SHT_NOBITS      8               /* Occupies no space in the file. */
#define SHT_REL         9               /* Relocations. */
#define SHT_SHLIB       10              /* reserved. */
#define SHT_DYNSYM      11              /* Symboltable used for dynamic loading. */
#define SHT_NUM         12              /* ? */
#define SHT_LOPROC      0x70000000L     /* Processor-spcific semantics lower bound. */
#define SHT_HIPROC      0x7fffffffL     /* Processor-spcific semantics upper bound. */
#define SHT_LOUSER      0x80000000L     /* Application program specific lower bound. */
#define SHT_HIUSER      0xffffffffL     /* Application program specific upper bound. */

/* sh_flags */
#define  SHF_WRITE      0x1             /* Writable during process execution. */
#define  SHF_ALLOC      0x2             /* The section occupies memory during process execution. */
#define  SHF_EXECINSTR  0x4             /* Executable machine instructs. */
#define  SHF_MASKPROC   0xf0000000L     /* Processor-specific semantics bits. */


/*
 * ELF Symbol table
 */
typedef struct                          /* 0x10 */
{
    Elf32_Word      st_name;            /* 0x00  Symbol name. (index) */
    Elf32_Addr      st_value;           /* 0x04  Value associated with the symbol. */
    Elf32_Word      st_size;            /* 0x08  Size associated with the symbol. */
    unsigned char   st_info;            /* 0x0c  Symbol type and binding attributes. */
    unsigned char   st_other;           /* 0x0d  Reserved. (currently 0?) */
    Elf32_Half      st_shndx;           /* 0x0e  Section index of related section. */
} Elf32_Sym;

/* symbol table index(es) */
#define STN_UNDEF       0               /* Reserved symboltable entry. */

/* st_info macros */
#define ELF32_ST_BIND(i)    ((i) >> 4)  /* Get the symbol binding attributes from the st_info member. */
#define ELF32_ST_TYPE(i)    ((i) & 0x0f)/* Get the symbol type from the st_info member. */
#define ELF32_ST_INFO(b,t)  ((b << 4) | (t & 0x0f)) /* Build a st_info member. */

/* st_info - binding attributes */
#define STB_LOCAL       0x0             /* Local symbol. Not visible outside the object file. */
#define STB_GLOBAL      0x1             /* Global symbol. Visible to everyone. */
#define STB_WEAK        0x2             /* Weak symbols resemble global symbols, but their definitions have lower precedence. */
#define STB_LOPROC      0xd             /* Processor-specific semantics lower bound. */
#define STB_HIPROC      0xf             /* Processor-specific semantics upper bound. */

/* st_info - types */
#define STT_NOTYPE      0x0             /* The symbol's type is not defined. */
#define STT_OBJECT      0x1             /* The symbol is associated with a data object, such as a variable an array, etc. */
#define STT_FUNC        0x2             /* The symbol is associated with a function or other executable code. */
#define STT_SECTION     0x3             /* The symbol is associated with a section. */
#define STT_FILE        0x4             /* Name of the source file... */
#define STT_LOPROC      0xd             /* Processor-specific semantics lower bound. */
#define STT_HIPROC      0xf             /* Processor-specific semantics upper bound. */


/*
 * ELF Relocation
 */
typedef struct                          /* 0x08 */
{
    Elf32_Addr      r_offset;           /* 0x00  Offset or virtual address. */
    Elf32_Word      r_info;             /* 0x04  Symbolindex/type. */
} Elf32_Rel;

typedef struct                          /* 0x0c */
{
    Elf32_Addr      r_offset;           /* 0x00  Offset or virtual address. */
    Elf32_Word      r_info;             /* 0x04  Symbolindex/type. */
    Elf32_Sword     r_addend;           /* 0x08  Constant addend. */
} Elf32_Rela;

/* r_info macro */
#define ELF32_R_SYM(i)      ((i) >> 8)  /* Gets symbol index from the r_info member. */
#define ELF32_R_TYPE(i)     ((unsigned char)(i)) /* Get the relocation type from the r_info member. */
#define ELF32_R_INFO(s,t)   (((s) << 8) | (unsigned char)(t)) /* Makes the r_info member. */

/* r_info - Intel 80386 relocations */
#define R_386_NONE      0               /*  */
#define R_386_32        1
#define R_386_PC32      2
#define R_386_GOT32     3
#define R_386_PLT32     4
#define R_386_COPY      5
#define R_386_GLOB_DAT  6
#define R_386_JMP_SLOT  7
#define R_386_RELATIVE  8
#define R_386_GOT_OFF   9
#define R_386_GOTPC     10
#define R_386_NUM       11



/*
 * ELF Program Header
 */
typedef struct                          /* 0x20 */
{
    Elf32_Word      p_type;             /* 0x00  Tells what this header describes or how to interpret it. */
    Elf32_Off       p_offset;           /* 0x04  Offset of the first byte of this segment. */
    Elf32_Addr      p_vaddr;            /* 0x08  Virtual address of the segment. */
    Elf32_Addr      p_paddr;            /* 0x0c  Physical address. Usually ignorable. */
    Elf32_Word      p_filesz;           /* 0x10  Count of bytes in the file image of this segment. Zero allowed. */
    Elf32_Word      p_memsz;            /* 0x14  Count of bytes in  the memory image of this segment. Zero allowed. */
    Elf32_Word      p_flags;            /* 0x18  Flags relevant to the segement. */
    Elf32_Word      p_align;            /* 0x1c  Alignment. 0 and 1 means no alignment. */
} Elf32_Phdr;

/* p_type - segment types */
#define PT_NULL         0               /* Unused header. */
#define PT_LOAD         1               /* Loadable segment. p_filesz, p_memsz and p_vaddr applies. */
#define PT_DYNAMIC      2               /* Dynamic linking information. */
#define PT_INTERP       3               /* Interpreter path. */
#define PT_NOTE         4               /* Auxiliary information. */
#define PT_SHLIB        5               /* Reserved. */
#define PT_PHDR         6               /* This header specifies the location and size of the program header table in file and memory. */
#define PT_LOPROC       0x70000000      /* Reserved processor-specific semantics range start. */
#define PT_HIPROC       0x7fffffff      /* Reserved processor-specific semantics range end (included). */

/* p_flags - permission flags */
#define PF_X            1               /* Executable */
#define PF_W            2               /* Writeable */
#define PF_R            4               /* Readable */


/*
 * Dynamic Structure
 */
typedef struct                          /* 0x08 */
{
    Elf32_Sword     d_tag;              /* 0x00 Tag type. */
    union
    {
        Elf32_Word  d_val;              /* 0x04 Value, interpreted according to the tag type. */
        Elf32_Addr  d_ptr;              /* 0x04 Virtual address, interpreted according to the tag type. */
    } d_un;
} Elf32_Dyn;

/* d_tag - tag types */                 /* d_un  exe so: '-' is ignored; '+' is mandatory; '*' is optional. */
#define DT_NULL         0               /*   -    -   -   Marks the end of the dynamic array. */
#define DT_NEEDED       1               /* d_val  *   *   This element holds the string table offset of a
                                                          null-terminated  string, giving the name of a needed
                                                          library. The offset is an index into the table
                                                          recoreded in the DT_STRTAB entry. */
#define DT_PLTRELSZ     2               /* d_val  *   *   This element holds the total size, in bytes, of the
                                                          relocation entries associated with the procedure
                                                          linkage table. If an entry of type DT_JMPREL is
                                                          present, a DT_PLTRELSZ must accompany it. */
#define DT_PLTGOT       3               /* d_ptr  *   *    */
#define DT_HASH         4               /* d_ptr  +   +    */
#define DT_STRTAB       5               /* d_ptr  +   +   This element holds the address of the string table.
                                                          Symbol names, library names, and other strings reside
                                                          in this table. */
#define DT_SYMTAB       6               /* d_ptr  +   +    */
#define DT_RELA         7               /* d_ptr  +   *    */
#define DT_RELASZ       8               /* d_val  +   *    */
#define DT_RELAENT      9               /* d_val  +   *    */
#define DT_STRSZ        10              /* d_val  +   +    */
#define DT_SYMENT       11              /* d_val  +   +    */
#define DT_INIT         12              /* d_ptr  *   *   This element holds the address of the initialization function. */
#define DT_FINI         13              /* d_ptr  *   *   This element holds the address of the termination function. */
#define DT_SONAME       14              /* d_val  -   *   This element holds the string table offset of a
                                                          null-terminated string, giving the name of the shared
                                                          object. The offset is an index into the table recorded
                                                          in the DT_STRTAB entry. */
#define DT_RPATH        15              /* d_val  +   +   This element holds the string table offset of a null-terminated
                                                          search library search path string. The offset is an index int
                                                          the table recorded in the DT_STRTAB entry. */
#define DT_SYMBOLIC     16              /*   -    -   *   This element's presence in a shared object library alters the
                                                          dynamic linker's symbol resolution algorithm for references
                                                          within the library. Instead of starting a symbol search with
                                                          the executable file, the dynamic linker starts from the shared
                                                          object file itself. If the shared object fails to supply the
                                                          referenced symbol, the dynamic linker then searches the
                                                          executable file and other shared objects as usual. */
#define DT_REL          17              /* d_ptr  +   *    */
#define DT_RELSZ        18              /* d_val  +   *    */
#define DT_RELENT       19              /* d_val  +   *    */
#define DT_PLTREL       20              /* d_val  *   *    */
#define DT_DEBUG        21              /* d_ptr  *   -    */
#define DT_TEXTREL      22              /*   -    *   *    */
#define DT_JMPREL       23              /* d_ptr  *   *    */
#define DT_LOPROC       0x70000000      /* Reserved processor-specific semantics range start. */
#define DT_HIPROC       0x7fffffff      /* Reserved processor-specific semantics range end (included). */

#pragma pack()
#endif /*_elf_h_*/
