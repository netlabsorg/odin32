/* $Id: elf2lx.h,v 1.1 1999-11-10 01:45:32 bird Exp $
 *
 * Elf2Lx - Declaration.
 *
 * Copyright (c) 1999 knut st. osmundsen
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

