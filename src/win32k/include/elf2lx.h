/* $Id: elf2lx.h,v 1.2 2000-02-26 00:46:31 bird Exp $
 *
 * Elf2Lx - Declaration.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _ELF2LX_H_
#define _ELF2LX_H_


class Elf2Lx : public ModuleBase
{
public:
    Elf2Lx(SFN hFile);
    ULONG   init(PCSZ pszFilename)


private:

};


#endif

