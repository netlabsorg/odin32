/* $Id: elf2lx.h,v 1.4 2000-12-11 06:53:51 bird Exp $
 *
 * Elf2Lx - Declaration.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _ELF2LX_H_
#define _ELF2LX_H_


/**
 * Elf to LX converter class.
 * @version
 * @verdesc
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @approval
 */
class Elf2Lx : public ModuleBase
{
public:
    /** @cat Constructor/Destructor */
    Elf2Lx(SFN hFile);
    ~Elf2Lx();

    /** @cat Public Main methods */
    ULONG  init(PCSZ pszFilename);
    ULONG  read(ULONG offLXFile, PVOID pvBuffer, ULONG cbToRead, ULONG flFlags, PMTE pMTE);
    ULONG  applyFixups(PMTE pMTE, ULONG iObject, ULONG iPageTable, PVOID pvPage,
                       ULONG ulPageAddress, PVOID pvPTDA); /*(ldrEnum32bitRelRecs)*/
    #ifndef RING0
    ULONG  writeFile(PCSZ pszLXFilename);
    #endif

    /** @cat public Helper methods */
    VOID   dumpVirtualLxFile();
    BOOL   queryIsModuleName(PCSZ pszFilename);

    /** @cat Static helper methods */
    static BOOL validHeader(Elf32_Ehdr *pEhdr);


private:
    /** @cat private data members. */

    /**
     * @cat Elf structures
     */
    Elf32_Ehdr *       pEhdr;
    Elf32_Phdr *       paPhdrs;

    /**
     * @cat LX structures
     */
    struct e32_exe     LXHdr;           /* Lxheader */

    struct o32_obj *   paObjTab;        /* Pointer to object table - if null check cObjects > 0 and generate it using makeObjectTable */
    struct o32_map *   paObjPageTab;    /* Pointer to object page table - if null check cObjects > 0 and generate it using makeObjectPageTable */
};


#endif

