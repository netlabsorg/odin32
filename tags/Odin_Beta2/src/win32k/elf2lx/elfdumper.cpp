/* $Id: elfdumper.cpp,v 1.3 2000-12-11 06:53:49 bird Exp $
 *
 * ELF dumper utility
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define TRUE  1
#define FALSE 0

#define PRINTCASE(Level,Value) \
    case Value: printData(Level, #Value); break

#define PRINTFLAG(Flags,Flag) \
    if (((Flags) & (Flag)) == (Flag)) \
        printData(#Flag " ")

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "elf.h"

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef unsigned long BOOL;
typedef struct
{
    BOOL    fdummy;
} OPTIONS, *POPTIONS;

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
BOOL fPrintLongData = FALSE;
BOOL fPrintLongDataIndented = FALSE;
int  iPrintLongDataIndentLevel = -1;

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static BOOL     read(FILE *phFile, void *pv, unsigned long cb, unsigned long off);
static void     syntax(void);
void *          loadELFImage(FILE *phFile);
int             dumpELFImage(void *pv);
int             dumpELFHdr(Elf32_Ehdr *pHdr32);

int             dumpELFSectionHeader(Elf32_Ehdr * pHdr32, int iShdr);
int             dumpELFSectionHeaderTable(Elf32_Ehdr * pHdr32);
char *          getELFSectionNameStringTable(Elf32_Ehdr * pHdr32);
int             dumpELFSymbolTable(Elf32_Ehdr * pHdr32,  Elf32_Shdr * pShdr);
int             dumpELFRelcations(Elf32_Ehdr * pHdr32,  Elf32_Shdr * pShdr);
int             dumpELFRelcationsA(Elf32_Ehdr * pHdr32,  Elf32_Shdr * pShdr);

int             dumpELFProgramHeader(Elf32_Ehdr * pHdr32, int iPhdr);
int             dumpELFProgramHeaderTable(Elf32_Ehdr * pHdr32);
int             dumpELFDynamicSegment(Elf32_Ehdr * pHdr32,  Elf32_Phdr * pPhdr);
int             getELFDynmaicTagValue(Elf32_Dyn *pDyn, Elf32_Sword tag, Elf32_Word *pword);
int             dumpELFInterpreterSegment(Elf32_Ehdr * pHdr32,  Elf32_Phdr * pPhdr);
int             dumpELFNoteSegment(Elf32_Ehdr * pHdr32,  Elf32_Phdr * pPhdr);



/* output helpers */
void printHeader(char *pszHeader);
void print(int iIndentLevel, char *pszName, char *pszDataFormat, ...);
void print(int iIndentLevel, char *pszText);
void printBeginLongData(int iIndentLevel);
void printEndLongData(void);
void printData(char *pszDataFormat, ...);
void printData(int iIndentLevel, char *pszDataFormat, ...);
int  printGetIndentData(int iIndentLevel);
int  printGetIndent(int iIndentLevel);
void printerr(const char *pszMsg, ...);


/**
 * Main function.
 * @returns   Number of signals.
 * @param     argc  Argument count.
 * @param     argv  Argument array.
 */
int main(int argc, char **argv)
{
    OPTIONS     options = {TRUE};
    BOOL        fFatal = FALSE;
    int         iRc = 0;
    int         argi;

    /**************************************************************************
    * parse arguments.
    * options:  -h or -?         Syntax help.
    **************************************************************************/
    argi = 1;
    while (argi < argc && !fFatal)
    {
        if(argv[argi][0] == '-' || argv[argi][0] == '/')
        {
            switch (argv[argi][1])
            {
                case 'h':
                case 'H':
                case '?':
                    syntax();
                    return 0;

                default:
                    printerr("incorrect parameter. (argi=%d, argv[argi]=%s)", argi, argv[argi]);
                    fFatal = TRUE;
                    break;
            }
        }
        else
        {
            FILE *phFile;

            phFile = fopen(argv[argi], "rb");
            if (phFile != NULL)
            {
                void *pv = loadELFImage(phFile);
                if (pv != NULL)
                {
                    dumpELFImage(pv);
                    free(pv);
                }
                fclose(phFile);
            }
            else
            {
                printerr("failed to open file '%s'.", argv[argi]);
                iRc++;
            }
        }
        argi++;
    }
    return iRc;
}


static void syntax(void)
{
    printf("\n"
           "ELFDumper v0.0.0 - \"Not Implemented\"\n"
           "------------------------------------\n"
           "syntax: ELFDumper.exe [-?] <files to dump>\n"
           );
}


/**
 * Loads an ELF image into memory for dumping.
 * @returns   Pointer to ELF image.
 * @param     phFile  File handle to the ELF image file.
 * @author    knut st. osmundsen
 */
void * loadELFImage(FILE *phFile)
{
    size_t cbFile;

    fseek(phFile, 0, SEEK_END);
    cbFile = (size_t)ftell(phFile);

    if (cbFile > sizeof(Elf32_Ehdr))
    {
        void *pv = malloc(cbFile);
        if (pv != NULL)
        {
            if (read(phFile, pv, cbFile, 0))
            {
                return pv;
            }
            else
                printerr("failed to read file.");
            free(pv);
        }
        else
            printerr("failed to allocate memory!. (%d bytes)", cbFile);
    }
    else
        printerr("file is less than the size of the elf header! (%d)", cbFile);
    return NULL;
}


/**
 * Dumps the loaded ELF image.
 * @returns   Error code. (0 is success)
 * @param     pv  Pointer to the base of the ...
 * @author    knut st. osmundsen
 */
int dumpELFImage(void *pv)
{
    Elf32_Ehdr *pHdr32 = (Elf32_Ehdr *)pv;
    int         rc = 0;

    if (*(unsigned long *)pv == ELFMAGICLSB)
    {
        rc = dumpELFHdr(pHdr32);
        if (rc == 0)
            rc = dumpELFSectionHeaderTable(pHdr32);
        if (rc == 0)
            rc = dumpELFProgramHeaderTable(pHdr32);
    }
    else
    {
        printerr("no ELF signature was found");
        rc = -1;
    }
    return rc;
}


/**
 * Dumps the ELF header.
 * @returns   0 on success, errorcode on error (-1).
 * @param     pHdr32  Pointer to the ELF32 header to dump.
 * @author    knut st. osmundsen
 */
int dumpELFHdr(Elf32_Ehdr *pHdr32)
{
    int i;
    int iRc = 0;
    printHeader("ELF Header");
    printBeginLongData(0);
    print(0, "e_ident");

    for (i = 0; i < EI_INDENT; i++)
        printData(i > 0 && i <= 3 ? "%c " : "%02x ", pHdr32->e_ident[i]);
    printEndLongData();

    for (i = 0; i < EI_INDENT; i++)
    {
        switch (i)
        {
            case EI_CLASS:
                switch (pHdr32->e_ident[i])
                {
                    PRINTCASE(0,ELFCLASS32);
                    case ELFCLASS64:
                        printData(0, "ELFCLASS64");
                        printerr("class not supported");
                        iRc = -1;
                        break;
                    case ELFCLASSNUM:
                        printData(0, "ELFCLASSNUM");
                        printerr("class not supported");
                        iRc = -1;
                        break;

                    default:
                        printData(0, "Invalid class 0x%02x", pHdr32->e_ident[i]);
                        printerr("class not supported");
                        iRc = -1;
                }
                break;
            case EI_DATA:
                switch (pHdr32->e_ident[i])
                {
                    PRINTCASE(0,ELFDATA2LSB);
                    case ELFDATA2MSB:
                        printData(0, "ELFDATA2MSB");
                        printerr("Data encoding not supported");
                        iRc = -1;
                        break;
                    default:
                        printData(0, "Invalid data encoding 0x%02x", pHdr32->e_ident[i]);
                        printerr("Data encoding not supported");
                        iRc = -1;
                }
                break;
            case EI_VERSION:
                switch (pHdr32->e_ident[i])
                {
                    PRINTCASE(0,EV_CURRENT);
                    PRINTCASE(0,EV_NUM);
                    default:
                        printData(0, "Unknown ELF version 0x%02x", pHdr32->e_ident[i]);
                        iRc = -1;
                }
                break;
        }
    }

    if (iRc == 0)
    {
        print(0, "e_type", "0x%04x", pHdr32->e_type);
        switch (pHdr32->e_type)
        {
            PRINTCASE(1,ET_NONE);
            PRINTCASE(1,ET_REL);
            PRINTCASE(1,ET_EXEC);
            PRINTCASE(1,ET_DYN);
            PRINTCASE(1,ET_CORE);
            //default:
        }
        print(0, "e_machine", "0x%04x", pHdr32->e_machine);
        switch (pHdr32->e_machine)
        {
            PRINTCASE(1,EM_NONE);
            PRINTCASE(1,EM_M32);
            PRINTCASE(1,EM_SPARC);
            PRINTCASE(1,EM_386);
            PRINTCASE(1,EM_68K);
            PRINTCASE(1,EM_88K);
            PRINTCASE(1,EM_486);
            PRINTCASE(1,EM_860);
            PRINTCASE(1,EM_MIPS);
            PRINTCASE(1,EM_MIPS_RS4_BE);
            PRINTCASE(1,EM_SPARC64);
            PRINTCASE(1,EM_PARISC);
            PRINTCASE(1,EM_SPARC32PLUS);
            PRINTCASE(1,EM_PPC);
            PRINTCASE(1,EM_ALPHA);
            //default:
        }
        print(0, "e_version", "%d (%s)", pHdr32->e_version,
              pHdr32->e_version == EV_CURRENT ? "EV_CURRENT"
              : pHdr32->e_version == EV_NUM ? "EV_NUM" : "unknown/invalid");
        print(0, "e_entry", "0x%08x", pHdr32->e_entry);
        print(0, "e_phoff", "0x%08x", pHdr32->e_phoff);
        print(0, "e_shoff", "0x%08x", pHdr32->e_shoff);
        print(0, "e_flags", "0x%08x", pHdr32->e_flags);
        print(0, "e_ehsize",    "%d (Current=%d)", pHdr32->e_ehsize, sizeof(Elf32_Ehdr));
        print(0, "e_phentsize", "%d (Current=%d)", pHdr32->e_phentsize, -1/* sizeof(Elf32_Phdr)*/);
        print(0, "e_phnum",     "0x%04x", pHdr32->e_phnum);
        print(0, "e_shentsize", "%d (Current=%d)", pHdr32->e_shentsize, sizeof(Elf32_Shdr));
        print(0, "e_shnum",     "0x%04x", pHdr32->e_shnum);
        print(0, "e_shstrndx",  "0x%04x", pHdr32->e_shstrndx);
    }

    return iRc;
}



/**
 * Dumps the section header table of an ELF image.
 * @returns   0 on success. Errorcode on error (-1).
 * @param     pHdr32  Pointer to the image.
 * @author    knut st. osmundsen
 */
int dumpELFSectionHeaderTable(Elf32_Ehdr * pHdr32)
{
    int          i;
    int          rc = 0;

    printHeader("Section Header Table");

    for (i = 0; i < pHdr32->e_shnum && rc == 0; i++)
    {
        if (i > 0)
            print(0,"");
        rc = dumpELFSectionHeader(pHdr32, i);
    }
    return rc;
}


/**
 * Dumps the section header table of an ELF image.
 * @returns   0 on success. Errorcode on error (-1).
 * @param     pHdr32  Pointer to the image.
 * @param     iShdr   Section number.
 * @author    knut st. osmundsen
 */
int dumpELFSectionHeader(Elf32_Ehdr * pHdr32, int iShdr)
{
    Elf32_Shdr * pShdr = (Elf32_Shdr*)((unsigned)pHdr32 + pHdr32->e_shoff + pHdr32->e_shentsize*iShdr);
    char *       paszStrings = getELFSectionNameStringTable(pHdr32);
    print(0, "Section Header", "no. %d  offset 0x%08x", iShdr, (unsigned)pShdr - (unsigned)pHdr32);
    if (paszStrings != NULL && pShdr->sh_name != 0)
        print(1, "sh_name", "%s (0x%01x)", paszStrings + pShdr->sh_name, pShdr->sh_name);
    else
        print(1, "sh_name", "0x%08x", pShdr->sh_name);
    print(1, "sh_type",     "0x%08x", pShdr->sh_type);
    switch (pShdr->sh_type)
    {
        PRINTCASE(1,SHT_NULL);
        PRINTCASE(1,SHT_PROGBITS);
        PRINTCASE(1,SHT_SYMTAB);
        PRINTCASE(1,SHT_STRTAB);
        PRINTCASE(1,SHT_RELA);
        PRINTCASE(1,SHT_HASH);
        PRINTCASE(1,SHT_DYNAMIC);
        PRINTCASE(1,SHT_NOTE);
        PRINTCASE(1,SHT_NOBITS);
        PRINTCASE(1,SHT_REL);
        PRINTCASE(1,SHT_SHLIB);
        PRINTCASE(1,SHT_DYNSYM);
        PRINTCASE(1,SHT_NUM);
        default:
            if (pShdr->sh_type >= SHT_LOPROC && pShdr->sh_type <= SHT_HIPROC)
                printData(1, "Processor-specific");
            else if (pShdr->sh_type >= SHT_LOUSER && pShdr->sh_type <= SHT_HIUSER)
                printData(1, "Application program specific");
            else
                printData(1, "unknown");
    }
    print(1, "sh_flags",    "0x%08x", pShdr->sh_flags);
    printBeginLongData(1);
    PRINTFLAG(pShdr->sh_flags,SHF_WRITE);
    PRINTFLAG(pShdr->sh_flags,SHF_ALLOC);
    PRINTFLAG(pShdr->sh_flags,SHF_EXECINSTR);
    if (pShdr->sh_flags & SHF_MASKPROC)
        printData("SHF_MASKPROC");
    printEndLongData();
    print(1, "sh_addr",     "0x%08x", pShdr->sh_addr);
    print(1, "sh_offset",   "0x%08x", pShdr->sh_offset);
    print(1, "sh_size",     "0x%08x", pShdr->sh_size);
    print(1, "sh_link",     "0x%08x", pShdr->sh_link);
    print(1, "sh_info",     "0x%08x", pShdr->sh_info);
    print(1, "sh_addralign", "0x%08x", pShdr->sh_addralign);
    print(1, "sh_entsize",  "0x%08x", pShdr->sh_entsize);

    /* process evt. known contents. */
    switch (pShdr->sh_type)
    {
        case SHT_SYMTAB:
        case SHT_DYNSYM:
            dumpELFSymbolTable(pHdr32, pShdr);
            break;
        case SHT_REL:
            dumpELFRelcations(pHdr32, pShdr);
            break;
        case SHT_RELA:
            break;
        case SHT_HASH:
            break;
        case SHT_NOTE:
            break;
    }

    return 0;
}


/**
 * Gets the Section Name String Table.
 * @returns   Pointer to the string table.
 * @param     pHdr32  Pointer to the image.
 * @author    knut st. osmundsen
 */
char *getELFSectionNameStringTable(Elf32_Ehdr * pHdr32)
{
    if (pHdr32->e_shstrndx != SHN_UNDEF)
    {
        Elf32_Shdr * pShdr = (Elf32_Shdr*)((unsigned)pHdr32 + pHdr32->e_shoff + pHdr32->e_shentsize * pHdr32->e_shstrndx);
        return (char*)((unsigned)pHdr32 + pShdr->sh_offset);
    }
    return NULL;
}


/**
 * Dumps a symboltable.
 * @returns   0 on success. Errorcode on error.
 * @param     pHdr32  Pointer to the image.
 * @param     pShdr   Pointer to the section containing the symboltable.
 * @author    knut st. osmundsen
 */
int dumpELFSymbolTable(Elf32_Ehdr * pHdr32,  Elf32_Shdr * pShdr)
{
    Elf32_Shdr * pShdrStr = (Elf32_Shdr*)((unsigned)pHdr32 + pHdr32->e_shoff + pHdr32->e_shentsize*pShdr->sh_link);
    Elf32_Sym  * pSym = (Elf32_Sym*)((unsigned)pHdr32 + pShdr->sh_offset);
    unsigned     off = 0;
    char *       paszStrings = (char *)((unsigned)pHdr32 + pShdrStr->sh_offset);

    /* Fixme?: sh_info - One greater than the symbol table index of the last local symbol (binding STB_LOCAL). */

    print(2, "Symbol table:");
    while (off < pShdr->sh_size)
    {
        flushall();
        print(2, "st_name",  "%s",     paszStrings + pSym->st_name);
        print(3, "st_value", "0x%08x", pSym->st_value);
        print(3, "st_size",  "%d",     pSym->st_size);
        printBeginLongData(3);
        print(3, "st_info",  "0x%02x", pSym->st_info);
        printData(" ");
        switch (ELF32_ST_BIND(pSym->st_info))
        {
            PRINTCASE(3,STB_LOCAL);
            PRINTCASE(3,STB_GLOBAL);
            PRINTCASE(3,STB_WEAK);
            default:
                printData("unknown binding");
        }
        printData(" ");
        switch (ELF32_ST_TYPE(pSym->st_info))
        {
            PRINTCASE(3,STT_NOTYPE);
            PRINTCASE(3,STT_OBJECT);
            PRINTCASE(3,STT_FUNC);
            PRINTCASE(3,STT_SECTION);
            PRINTCASE(3,STT_FILE);
            default:
                printData("unknown type");
        }
        printEndLongData();
        print(3, "st_other", "0x%02x", pSym->st_other);
        print(3, "st_shndx", "0x%04x", pSym->st_shndx);

        /* next */
        pSym = (Elf32_Sym*)((unsigned)pSym + pShdr->sh_entsize);
        off += pShdr->sh_entsize;
    }
    return 0;
}


int dumpELFRelcations(Elf32_Ehdr * pHdr32,  Elf32_Shdr * pShdr)
{
    Elf32_Rel  * pRel = (Elf32_Rel*)((unsigned)pHdr32 + pShdr->sh_offset);
    unsigned     off = 0;

    while (off < pShdr->sh_size)
    {
        printBeginLongData(2);
        print(2, "Relocation", "off=0x%08x  type=", pRel->r_offset);
        switch (ELF32_R_TYPE(pRel->r_info))
        {
            PRINTCASE(2,R_386_NONE);
            PRINTCASE(2,R_386_32);
            PRINTCASE(2,R_386_PC32);
            PRINTCASE(2,R_386_GOT32);
            PRINTCASE(2,R_386_PLT32);
            PRINTCASE(2,R_386_COPY);
            PRINTCASE(2,R_386_GLOB_DAT);
            PRINTCASE(2,R_386_JMP_SLOT);
            PRINTCASE(2,R_386_RELATIVE);
            PRINTCASE(2,R_386_GOT_OFF);
            PRINTCASE(2,R_386_GOTPC);
            default:
                printData("<unknown type>");
        }
        if (ELF32_R_SYM(pRel->r_info) != STN_UNDEF)
            printData(" sym=STN_UNDEF");
        else
            printData(" sym=0x%08x", ELF32_R_SYM(pRel->r_info));
        printEndLongData();

        /* next */
        pRel = (Elf32_Rel*)((unsigned)pRel + pShdr->sh_entsize);
        off += pShdr->sh_entsize;
    }
    return 0;
}

int dumpELFRelcationsA(Elf32_Ehdr * pHdr32,  Elf32_Shdr * pShdr)
{
    Elf32_Rela * pRela = (Elf32_Rela*)((unsigned)pHdr32 + pShdr->sh_offset);
    unsigned     off = 0;

    while (off < pShdr->sh_size)
    {
        printBeginLongData(2);
        print(2, "Relocation+Addend", "off=0x%08x  type=", pRela->r_offset);
        switch (ELF32_R_TYPE(pRela->r_info))
        {
            PRINTCASE(2,R_386_NONE);
            PRINTCASE(2,R_386_32);
            PRINTCASE(2,R_386_PC32);
            PRINTCASE(2,R_386_GOT32);
            PRINTCASE(2,R_386_PLT32);
            PRINTCASE(2,R_386_COPY);
            PRINTCASE(2,R_386_GLOB_DAT);
            PRINTCASE(2,R_386_JMP_SLOT);
            PRINTCASE(2,R_386_RELATIVE);
            PRINTCASE(2,R_386_GOT_OFF);
            PRINTCASE(2,R_386_GOTPC);
            default:
                printData("<unknown type>");
        }
        if (ELF32_R_SYM(pRela->r_info) != STN_UNDEF)
            printData(" sym=STN_UNDEF");
        else
            printData(" sym=0x%08x", ELF32_R_SYM(pRela->r_info));
        printData(" add=0x%08x", pRela->r_addend);
        printEndLongData();

        /* next */
        pRela = (Elf32_Rela*)((unsigned)pRela + pShdr->sh_entsize);
        off += pShdr->sh_entsize;
    }
    return 0;
}


/**
 * Dumps the program header tables.
 * @returns   0 on success. Errorcode on error (-1).
 * @param     pHdr32  Pointer to the image.
 * @author    knut st. osmundsen
 */
int dumpELFProgramHeaderTable(Elf32_Ehdr * pHdr32)
{
    int          i;
    int          rc = 0;

    printHeader("Program Header Table");

    for (i = 0; i < pHdr32->e_phnum && rc == 0; i++)
    {
        if (i > 0)
            print(0,"");
        rc = dumpELFProgramHeader(pHdr32, i);
    }
    return rc;
}


/**
 * Dumps a program header of an ELF image.
 * @returns   0 on success. Errorcode on error (-1).
 * @param     pHdr32  Pointer to the image.
 * @param     iPhdr   Program header number.
 * @author    knut st. osmundsen
 */
int dumpELFProgramHeader(Elf32_Ehdr * pHdr32, int iPhdr)
{
    Elf32_Phdr * pPhdr = (Elf32_Phdr*)((unsigned)pHdr32 + pHdr32->e_phoff + pHdr32->e_phentsize*iPhdr);


    print(0, "Program Header", "no. %d  offset 0x%08x", iPhdr, (unsigned)pPhdr - (unsigned)pHdr32);

    print(1, "p_type",      "0x%08x", pPhdr->p_type);
    switch (pPhdr->p_type)
    {
        PRINTCASE(1,PT_NULL   );
        PRINTCASE(1,PT_LOAD   );
        PRINTCASE(1,PT_DYNAMIC);
        PRINTCASE(1,PT_INTERP );
        PRINTCASE(1,PT_NOTE   );
        PRINTCASE(1,PT_SHLIB  );
        PRINTCASE(1,PT_PHDR   );
        default:
            if (pPhdr->p_type >= PT_LOPROC && pPhdr->p_type <= PT_HIPROC)
                printData(1, "Processor-specific");
            else
                printData(1, "unknown");
    }
    print(1, "p_offset",    "0x%08x", pPhdr->p_offset);
    print(1, "p_vaddr",     "0x%08x", pPhdr->p_vaddr);
    print(1, "p_paddr",     "0x%08x", pPhdr->p_paddr);
    print(1, "p_filesz",    "0x%08x", pPhdr->p_filesz);
    print(1, "p_memsz",     "0x%08x", pPhdr->p_memsz);
    print(1, "p_flags",     "0x%08x", pPhdr->p_flags);
    printBeginLongData(1);
    PRINTFLAG(pPhdr->p_flags, PF_X);
    PRINTFLAG(pPhdr->p_flags, PF_W);
    PRINTFLAG(pPhdr->p_flags, PF_R);
    printEndLongData();
    print(1, "p_align",     "0x%08x", pPhdr->p_align);
    printBeginLongData(1);
    if (pPhdr->p_align &&
        (pPhdr->p_vaddr % pPhdr->p_align != pPhdr->p_offset % pPhdr->p_align))
        printData(1, "alignment error?");
    printEndLongData();


    switch (pPhdr->p_type)
    {
        case PT_DYNAMIC:
            dumpELFDynamicSegment(pHdr32, pPhdr);
            break;

        case PT_INTERP:
            dumpELFInterpreterSegment(pHdr32, pPhdr);
            break;

        case PT_NOTE:
            dumpELFNoteSegment(pHdr32,  pPhdr);
            break;
    }
    return 0;
}


/**
 * Dumps a dynamic segment.
 * @returns   0 on success.
 * @param     pHdr32  Pointer to ELF header (and base of the loaded file).
 * @param     pPhdr   Pointer to program header.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int dumpELFDynamicSegment(Elf32_Ehdr * pHdr32,  Elf32_Phdr * pPhdr)
{
    Elf32_Word      word;
    Elf32_Addr      base;
    int             i;
    Elf32_Dyn *     pDyn = (Elf32_Dyn*) ((unsigned)pHdr32 + pPhdr->p_offset);
    Elf32_Phdr *    pPhdrs = (Elf32_Phdr*)((unsigned)pHdr32 + pHdr32->e_phoff);
    char *          paszStrings;

    /* find base address */
    i = 0;
    while (i < pHdr32->e_phnum && pPhdrs->p_type != PT_LOAD)
    {
        i++;
        pPhdrs = (Elf32_Phdr*)((unsigned)pPhdrs + pHdr32->e_phentsize);
    }
    if (i < pHdr32->e_phnum)
        base = pPhdrs->p_vaddr;

    /* find string table */
    if (getELFDynmaicTagValue(pDyn, DT_STRTAB, &word) == 0)
        paszStrings = (char*)(word - base + (int)pHdr32);
    else
        paszStrings = NULL;             /* no string table - funny! */

    /*
     * Loop thru the entries.
     */
    i = 0;
    while (pDyn[i].d_tag != DT_NULL)
    {
        switch (pDyn[i].d_tag)
        {
            case DT_NULL:       /*   -    -   -   Marks the end of the dynamic array. */
                break;
            case DT_NEEDED:     /* d_val  *   *   Holds the string table offset of a null-terminated
                                                  string, giving the name of a needed library. The
                                                  offset is an index into the table recoreded in the
                                                  DT_STRTAB entry. */
                print(2, "DT_NEEDED", "0x%08x", pDyn[i].d_un.d_val);
                if (paszStrings != NULL)
                    printData(2, "%s", paszStrings + pDyn[i].d_un.d_val);
                else
                    printData(2, "(no string table)");
                break;

            case DT_PLTRELSZ:
                print(2, "DT_PLTRELSZ", "0x%08x", pDyn[i].d_un.d_val);
                break;

            case DT_PLTGOT:
                print(2, "DT_PLTGOT", "0x%08x", pDyn[i].d_un.d_val);
                break;
            case DT_HASH:
                print(2, "DT_HASH", "0x%08x", pDyn[i].d_un.d_val);
                break;
            case DT_STRTAB:
                print(2, "DT_STRTAB", "0x%08x", pDyn[i].d_un.d_val);
                break;
            case DT_SYMTAB:
                print(2, "DT_SYMTAB", "0x%08x", pDyn[i].d_un.d_val);
                break;
            case DT_RELA:
                print(2, "DT_RELA", "0x%08x", pDyn[i].d_un.d_val);
                break;
            case DT_RELASZ:
                print(2, "DT_RELASZ", "0x%08x", pDyn[i].d_un.d_val);
                break;
            case DT_RELAENT:
                print(2, "DT_RELAENT", "0x%08x", pDyn[i].d_un.d_val);
                break;

            case DT_STRSZ:      /* d_ptr  +   +   This element holds the address of the string table.
                                                  Symbol names, library names, and other strings reside
                                                  in this table. */
                print(2, "DT_STRSZ", "0x%08x", pDyn[i].d_un.d_ptr);
                break;

            case DT_SYMENT:
                print(2, "DT_SYMENT", "0x%08x", pDyn[i].d_un.d_val);
                break;

            case DT_INIT:       /* d_ptr  *   *   This element holds the address of the initialization function. */
                print(2, "DT_INIT", "0x%08x (init function)", pDyn[i].d_un.d_val);
                break;

            case DT_FINI:       /* d_ptr  *   *   This element holds the address of the termination function. */
                print(2, "DT_FINI", "0x%08x (term function)", pDyn[i].d_un.d_val);
                break;

            case DT_SONAME:     /* d_val  -   *   This element holds the string table offset of a
                                                  null-terminated string, giving the name of the shared
                                                  object. The offset is an index into the table recorded
                                                  in the DT_STRTAB entry. */

                print(2, "DT_SONAME", "0x%08x", pDyn[i].d_un.d_val);
                if (paszStrings != NULL)
                    printData(2, "%s", paszStrings + pDyn[i].d_un.d_val);
                else
                    printData(2, "(no string table)");
                break;

            case DT_RPATH:      /* d_val  +   +   This element holds the string table offset of a null-terminated
                                                  search library search path string. The offset is an index int
                                                  the table recorded in the DT_STRTAB entry. */
                print(2, "DT_RPATH", "0x%08x", pDyn[i].d_un.d_val);
                if (paszStrings != NULL)
                    printData(2, "%s", paszStrings + pDyn[i].d_un.d_val);
                else
                    printData(2, "(no string table)");
                break;

            case DT_SYMBOLIC:   /*   -    -   *   This element's presence in a shared object library alters the
                                                  dynamic linker's symbol resolution algorithm for references
                                                  within the library. Instead of starting a symbol search with
                                                  the executable file, the dynamic linker starts from the shared
                                                  object file itself. If the shared object fails to supply the
                                                  referenced symbol, the dynamic linker then searches the
                                                  executable file and other shared objects as usual. */
                print(2, "DT_SYMBOLIC", "(%d)", pDyn[i].d_un.d_val);
                printData(2, "Searches within the library before the executable.");
                break;

            case DT_REL:
                print(2, "DT_REL", "0x%08x", pDyn[i].d_un.d_val);
                break;
            case DT_RELSZ:
                print(2, "DT_RELSZ", "0x%08x", pDyn[i].d_un.d_val);
                break;
            case DT_RELENT:
                print(2, "DT_RELENT", "0x%08x", pDyn[i].d_un.d_val);
                break;
            case DT_PLTREL:
                print(2, "DT_PLTREL", "0x%08x", pDyn[i].d_un.d_val);
                break;
            case DT_DEBUG:
                print(2, "DT_DEBUG", "0x%08x", pDyn[i].d_un.d_val);
                break;
            case DT_TEXTREL:
                print(2, "DT_TEXTREL", "0x%08x", pDyn[i].d_un.d_val);
                break;
            case DT_JMPREL:
                print(2, "DT_JMPREL", "0x%08x", pDyn[i].d_un.d_val);
                break;

        }
        /* next */
        i++;
    }
    return 0;
}


/**
 *
 * @returns   0 on succes.
 * @param     pDyn   Pointer to start of tag array.
 * @param     tag    Tag type to find.
 * @param     pword  Pointer to Elf32_Word which will hold the tag value if found.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int getELFDynmaicTagValue(Elf32_Dyn *pDyn, Elf32_Sword tag, Elf32_Word *pword)
{
    while (pDyn->d_tag != tag && pDyn->d_tag != DT_NULL)
        pDyn++;
    if (pDyn->d_tag != DT_NULL)
    {
        *pword = pDyn->d_un.d_val;
        return 0;
    }
    return -1;
}


/**
 * Dumps a Interpreter segment.
 * @returns   0 on success.
 * @param     pHdr32  Pointer to ELF header (and base of the loaded file).
 * @param     pPhdr   Pointer to program header.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int dumpELFInterpreterSegment(Elf32_Ehdr * pHdr32,  Elf32_Phdr * pPhdr)
{
    print(2, "Interpreter name",  "%s",     (char*)pHdr32 + pPhdr->p_offset);
    return 0;
}


/**
 * Dumps a note segment.
 * @returns   0 on success.
 * @param     pHdr32  Pointer to ELF header (and base of the loaded file).
 * @param     pPhdr   Pointer to program header.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int dumpELFNoteSegment(Elf32_Ehdr * pHdr32,  Elf32_Phdr * pPhdr)
{
    pPhdr = pPhdr;
    pHdr32 = pHdr32;
    return 0;
}





/*
 * Helper functions
 */

/**
 * Reads for the given file at a specific offset.
 * @returns   Success indicator (TRUE/FALSE).
 * @param     phFile  File handle.
 * @param     pv      Pointer to output buffer.
 * @param     cb      The count of bytes to read.
 * @param     off     Offset (from the begining of the file) to start reading from.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
static BOOL  read(FILE *phFile, void *pv, unsigned long cb, unsigned long off)
{
    if (fseek(phFile, off, SEEK_SET) == 0)
    {
        if (fread(pv, (size_t)cb, 1, phFile) == 1)
            return 1;
        else
            return 0;
    }
    else
        return 0;
}

void printHeader(char *pszHeader)
{
    static BOOL fFirst = TRUE;
    if (!fFirst)
        putchar('\n');
    else
        fFirst = FALSE;

    int i = strlen(pszHeader);
    puts(pszHeader);
    for (i=i; i >= 0; i--)
        putchar('-');
    putchar('\n');
}


void print(int iIndentLevel, char *pszName, char *pszDataFormat, ...)
{
    va_list arg;
    int i = printf("%*s%s", printGetIndent(iIndentLevel), "", pszName);
    i = printGetIndentData(iIndentLevel) - i;
    for (i = i > 0 ? i : 1;  i > 0;  i--)
        putchar(' ');
    va_start(arg, pszDataFormat);
    vprintf(pszDataFormat, arg);
    va_end(arg);

    if (!fPrintLongData)
    {
        putchar('\n');
        fPrintLongDataIndented = FALSE;
    }
    else
        fPrintLongDataIndented = TRUE;
}

void print(int iIndentLevel, char *pszText)
{
    int i = printf("%*s%s", printGetIndent(iIndentLevel), "", pszText);
    if (!fPrintLongData)
    {
        putchar('\n');
        fPrintLongDataIndented = FALSE;
    }
    else
    {
        i = printGetIndentData(iIndentLevel) - i;
        printf("%*s", i > 0 ? i : 1, "");
        fPrintLongDataIndented = TRUE;
    }
}

void printBeginLongData(int iIndentLevel)
{
    assert(!fPrintLongData);
    fPrintLongData = TRUE;
    iPrintLongDataIndentLevel = iIndentLevel;
    fPrintLongDataIndented = FALSE;
}

void printEndLongData(void)
{
    assert(fPrintLongData == TRUE);
    fPrintLongData = FALSE;
    putchar('\n');
}

void printData(char *pszDataFormat, ...)
{
    va_list arg;

    if (!fPrintLongDataIndented)
    {
        printf("%*s", printGetIndentData(iPrintLongDataIndentLevel), "");
        fPrintLongDataIndented = TRUE;
    }

    va_start(arg, pszDataFormat);
    vprintf(pszDataFormat, arg);
    va_end(arg);
}


void printData(int iIndentLevel, char *pszDataFormat, ...)
{
    va_list arg;

    if (!fPrintLongDataIndented)
    {
        printf("%*s", printGetIndentData(iIndentLevel), "");
        fPrintLongDataIndented = TRUE;
    }

    va_start(arg, pszDataFormat);
    vprintf(pszDataFormat, arg);
    va_end(arg);

    if (!fPrintLongData)
        putchar('\n');
}

int printGetIndentData(int iIndentLevel)
{
    static int idata[4] = {23, 27, 30, 33};
    assert(iIndentLevel < 4);
    return idata[iIndentLevel];
}

int printGetIndent(int iIndentLevel)
{
    static int idata[4] = {3, 6, 9, 12};
    assert(iIndentLevel < 4);
    return idata[iIndentLevel];
}

void printerr(const char *pszMsg, ...)
{
    va_list arg;

    printf("error: ");

    va_start(arg, pszMsg);
    vprintf(pszMsg, arg);
    va_end(arg);

    putchar('\n');
}

