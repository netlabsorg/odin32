/*		
 * Common controls functions
 *
 */

#include <windows.h>
#include <misc.h>

DWORD WINAPI COMCTL32_389(DWORD x1, DWORD x2)
{
   dprintf(("comctl32_389: %x %X not implemented!!", x1, x2));
   return 0; //NT 4 comctl32 returns 0
}

DWORD WINAPI COMCTL32_390(DWORD x1, DWORD x2, DWORD x3, DWORD x4)
{
  dprintf(("comctl32_390: %x %x %x %x not implemented!!", x1, x2, x3, x4));

  /* Pseudo Assembler
   push [esp+arg_C]
   mov  eax, [esp+4+arg_0]
   push [esp+4+arg_8]
   mov  dword ptr [eax+1A8h], 1
   push [esp+8+arg_4]
   push dword ptr [eax + 38h]
   call ds:SetDIBColorTable
   retn 10h
  */

   return 0;
}
