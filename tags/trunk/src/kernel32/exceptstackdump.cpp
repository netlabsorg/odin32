/* $Id: exceptstackdump.cpp,v 1.4 2001-07-20 15:33:29 sandervl Exp $ */
/*
 * Stack dump code
 *
 * Based on XFolder's stack dumping facility:
 *      	  Copyright (C) 1992-99 Ulrich M”ller,
 *                                      Kim Rasmussen,
 *                                      Marc Fiammante,
 *                                      John Currier,
 *                                      Anthony Cruise.
 *
 */

#define INCL_MISC
#define INCL_BASE
#include <os2.h>                     //No wrappers needed
#include <stdio.h>
#include <string.h>
#include <misc.h>
#include "exceptstackdump.h"
#include "exceptutil.h"
#include "oslibmisc.h"
#include "winexebase.h"
#include "windllbase.h"

#define DBG_LOCALLOG	DBG_exceptstackdump
#include "dbglocal.h"

int SYSTEM EXPORT WriteLogNoEOL(char *tekst, ...);

#undef dprintf
#define dprintf(a)     if(DbgEnabledKERNEL32[DBG_LOCALLOG] == 1) WriteLogNoEOL a

/* ******************************************************************
 *                                                                  *
 *   PART 3: ANALYZE SYMBOL (.SYM) FILE                             *
 *                                                                  *
 ********************************************************************/

/*
 *@@ dbgPrintSYMInfo:
 *      this gets called by dbgPrintStack to check if a SYM
 *      file with the same filename exists and try to
 *      get the info from there.
 *
 *      This gets called for every line of the stack
 *      walk, but only if getting the information from
 *      the debug code failed, e.g. because no debug code
 *      was available.
 *      The file pointer is in the "Source file" column
 *      every time.
 *
 *      New with V0.84.
 */

BOOL dbgPrintSYMInfo(CHAR * SymFileName, ULONG Object, ULONG TrapOffset)
{
    static FILE    *SymFile;
    static MAPDEF   MapDef;
    static SEGDEF   SegDef;
    static SEGDEF  *pSegDef;
    static SYMDEF32 SymDef32;
    static SYMDEF16 SymDef16;
    static char     Buffer[256];
    static int      SegNum, SymNum, LastVal;
    static unsigned long int SegOffset,
                    SymOffset, SymPtrOffset;
    int rc = FALSE;

    // open .SYM file
    SymFile = fopen(SymFileName, "rb");
    if (SymFile == 0)
    {
//        dprintf(("Could not open symbol file %s", SymFileName));
	goto endofprintsym;
    }                           // endif

    // read in first map definition
    fread(&MapDef, sizeof(MAPDEF), 1, SymFile);

    SegOffset = SEGDEFOFFSET(MapDef);

    // go thru all segments
    for (SegNum = 0; SegNum < MapDef.cSegs; SegNum++)
    {
        // printf("Scanning segment #%d Offset %4.4hX",SegNum+1,SegOffset);
        if (fseek(SymFile, SegOffset, SEEK_SET))
        {
            dprintf(("Seek error."));
	    goto endofprintsym;
        }

        // read in segment definition
        fread(&SegDef, sizeof(SEGDEF), 1, SymFile);
        if (SegNum == Object)
        {
            // stack object found:
            Buffer[0] = 0x00;
            LastVal = 0;

            // go thru all symbols in this object
            for (SymNum = 0; SymNum < SegDef.cSymbols; SymNum++)
            {
                // read in symbol offset USHORT
                SymPtrOffset = SYMDEFOFFSET(SegOffset, SegDef, SymNum);
                fseek(SymFile, SymPtrOffset, SEEK_SET);
                fread(&SymOffset, sizeof(unsigned short int), 1, SymFile);

                // go to symbol definition
                fseek(SymFile, SymOffset + SegOffset, SEEK_SET);

                if (SegDef.bFlags & 0x01)
                {
                    // 32-bit symbol:
                    fread(&SymDef32, sizeof(SYMDEF32), 1, SymFile);
#if 0
                    if (SymDef32.wSymVal == TrapOffset)
                    {
                    	Buffer[0] = SymDef32.achSymName[0];
                    	fread(&Buffer[1], 1, SymDef32.cbSymName, SymFile);
                    	Buffer[SymDef32.cbSymName] = 0x00;
                        dprintf(("%s\n", Buffer));
			rc = TRUE;
			break;
		    }
#endif
		    //SvL: If it's an exact match, then the app put a procedure
                    //     address on the stack; ignore that
		    if (LastVal == TrapOffset) {
			break;
		    }
                    if (SymDef32.wSymVal > TrapOffset)
                    {
                        // symbol found
                        dprintf(("between %s + 0x%X", Buffer, TrapOffset - LastVal));
                    }
                    LastVal = SymDef32.wSymVal;
                    Buffer[0] = SymDef32.achSymName[0];
                    fread(&Buffer[1], 1, SymDef32.cbSymName, SymFile);
                    Buffer[SymDef32.cbSymName] = 0x00;

                    if (SymDef32.wSymVal > TrapOffset)
                    {
                        // symbol found, as above
                        dprintf(("  and %s - 0x%X\n", Buffer, LastVal - TrapOffset));
			rc = TRUE;
                        break;
		    }
                    /*printf("32 Bit Symbol <%s> Address %p",Buffer,SymDef32.wSymVal); */
                }
                else
                {
                    // 16-bit symbol:
                    fread(&SymDef16, sizeof(SYMDEF16), 1, SymFile);

		    //SvL: If it's an exact match, then the app put a procedure
                    //     address on the stack; ignore that
		    if (LastVal == TrapOffset) {
			break;
		    }

                    if (SymDef16.wSymVal > TrapOffset)
                    {
                        dprintf(("between %s + %X", Buffer, TrapOffset - LastVal));
                    }
                    LastVal = SymDef16.wSymVal;
                    Buffer[0] = SymDef16.achSymName[0];
                    fread(&Buffer[1], 1, SymDef16.cbSymName, SymFile);
                    Buffer[SymDef16.cbSymName] = 0x00;
                    if (SymDef16.wSymVal > TrapOffset)
                    {
                        dprintf(("  and %s - %X\n", Buffer, LastVal - TrapOffset));
			rc = TRUE;
                        break;
                    }
                    /*printf("16 Bit Symbol <%s> Address %p",Buffer,SymDef16.wSymVal); */
                }               // endif
            }
            break;
        }                       // endif
        SegOffset = NEXTSEGDEFOFFSET(SegDef);
    }                           // endwhile
endofprintsym:
    if(SymFile) fclose(SymFile);
    if(rc == FALSE) dprintf(("\n"));
    return rc;
}
//******************************************************************************
//******************************************************************************
void dbgPrintStack(PEXCEPTIONREPORTRECORD       pERepRec,
                   PEXCEPTIONREGISTRATIONRECORD pERegRec,
                   PCONTEXTRECORD               pCtxRec,
                   PVOID                        p)
{
 ULONG  Size, Attr, hMod, ObjNum, Offset, addr;
 APIRET rc;
 PTIB   ptib = NULL;
 PPIB   ppib = NULL;
 CHAR   Name[CCHMAXPATH] = "unknown";
 ULONG *stacktop, *stackbase;
 UCHAR  instr;
 static BOOL fEntered = FALSE;

  if(DosGetInfoBlocks(&ptib, &ppib) || ptib == NULL) {
	return;
  }
  if(fEntered == TRUE) {
	return;
  }

  fEntered = TRUE;

  stacktop  = (ULONG *)ptib->tib_pstacklimit - 4;
  stackbase = (ULONG *)ptib->tib_pstack;
  if(pCtxRec->ctx_RegEsp < (ULONG)stacktop && pCtxRec->ctx_RegEsp > (ULONG)stackbase)
  {
	stackbase = (ULONG *)pCtxRec->ctx_RegEsp;
  }
  else
  if(pCtxRec->ctx_RegEbp < (ULONG)stacktop && pCtxRec->ctx_RegEbp > (ULONG)stackbase)
  {
	stackbase = (ULONG *)pCtxRec->ctx_RegEbp;
  }
  else  stackbase = (ULONG *)getESP();

  dprintf(("** BEGIN STACK DUMP **\n"));
  while(stacktop > stackbase) 
  {
    	Size = 10;
     	rc = DosQueryMem((PVOID) (stacktop), &Size, &Attr);
       	if(rc != NO_ERROR || !(Attr & PAG_COMMIT) || (Size < 4))
       	{
       		break;
        }

	addr = *stacktop;
	Size = 16;
     	rc = DosQueryMem((PVOID) (*stacktop-16), &Size, &Attr);
      	if(rc != NO_ERROR || ((Attr & (PAG_EXECUTE|PAG_READ|PAG_COMMIT)) != (PAG_EXECUTE|PAG_READ|PAG_COMMIT)) || (Size < 16)) {
//		dprintf(("skiploop %x (rc %x, Attr %x Size %d)\n", *stacktop, rc, Attr, Size));
		if(WinExe && WinExe->insideModule(addr) && WinExe->insideModuleCode(addr)) {
			sprintf(Name, "%s", WinExe->getModuleName());
			dprintf(("%-13s      at 0x%08x\n", Name, addr));
		}
		else {
		  	Win32DllBase *dll = Win32DllBase::findModuleByAddr(addr);
			if(dll && dll->insideModuleCode(addr)) {
				sprintf(Name, "%s", dll->getModuleName());
				dprintf(("%-13s      at 0x%08x\n", Name, addr));
			}
		}

		goto skiploop;
	}
#if 0
	instr = *(UCHAR *)(((ULONG)*stacktop) - 5);
	if(instr == 0xE8) {//call with displacement
		addr = *(ULONG *)(((ULONG)*stacktop)-4);
		addr+= (((ULONG)*stacktop)-5);
  	}
//	else {
//	if(*(UCHAR *)(((ULONG)*stacktop) - 6) == 0xff) {
//		modrm = *(UCHAR *)(((ULONG)*stacktop) - 5);
//        }
	else
#endif
	addr = *stacktop;

        rc = DosQueryModFromEIP(&hMod, &ObjNum, sizeof(Name),
                                Name, &Offset, addr);
        if(rc == NO_ERROR && ObjNum != -1)
        {
             	DosQueryModuleName(hMod, sizeof(Name), Name);

	     	int namelen = strlen(Name);
             	if(namelen > 3)
             	{
			if(addr > (ULONG)ptib->tib_pstacklimit || addr < (ULONG)ptib->tib_pstack) {
             			// print module and object
             			dprintf(("%-13s %04X at 0x%08x ", OSLibStripPath(Name), ObjNum + 1, addr));

	             		strcpy(Name + namelen - 3, "SYM");
		                dbgPrintSYMInfo(Name, ObjNum, Offset);
			}
             	}
             	else
             	{
			dprintf(("*Unknown* at %x\n", addr));
             	}
        }
	else {
		if(WinExe && WinExe->insideModule(addr) && WinExe->insideModuleCode(addr)) {
			sprintf(Name, "%s", WinExe->getModuleName());
			dprintf(("%-13s      at 0x%08x\n", Name, addr));
		}
		else {
		  	Win32DllBase *dll = Win32DllBase::findModuleByAddr(addr);
			if(dll && dll->insideModuleCode(addr)) {
				sprintf(Name, "%s", dll->getModuleName());
				dprintf(("%-13s      at 0x%08x\n", Name, addr));
			}
		}
//		dprintf(("unknown %x at 0x%x\n", stacktop, addr));
	}
skiploop:
	stacktop -= 1;
  } //while

  addr = pCtxRec->ctx_RegEip;   
  if(WinExe && WinExe->insideModule(addr) && WinExe->insideModuleCode(addr)) {
     sprintf(Name, "%s", WinExe->getModuleName());
     dprintf(("%-13s      at 0x%08x\n", Name, addr));
  }
  else {
     Win32DllBase *dll = Win32DllBase::findModuleByAddr(addr);
     if(dll && dll->insideModuleCode(addr)) {
	sprintf(Name, "%s", dll->getModuleName());
	dprintf(("%-13s      at 0x%08x\n", Name, addr));
     }
  }

  dprintf(("** END OF STACK DUMP **\n"));
  fEntered = FALSE;
}
//******************************************************************************
//******************************************************************************
