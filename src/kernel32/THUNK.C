/* $Id: THUNK.C,v 1.1 1999-05-24 20:19:42 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 thunking API functions (mostly stubs)
 *
 * Copyright 1998 Patrick Haller (stubs + Wine port) (?)
 *
 * Original WINE code (win32\kernel32.c)
 *
 * KERNEL32 thunks and other undocumented stuff
 *
 * Copyright 1997-1998 Marcus Meissner
 * Copyright 1998      Ulrich Weigand
 */
#include <os2win.h>
#include <string.h>
#include <builtin.h>
#include "thunk.h"

//******************************************************************************
//******************************************************************************
DWORD WIN32API MapLS(void *address)
{
//  _interrupt(3);
  dprintf(("MapLS %X, not supported\n", address));
  return((DWORD)address);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API ThunkProc(DWORD arg1)
{
  dprintf(("ThunkProc - stub\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
void WIN32API FT_Prolog(CONTEXT *context)
{
  dprintf(("FT_Prolog - stub\n"));
}
//******************************************************************************
//******************************************************************************
/**********************************************************************
 *      QT_Thunk            (KERNEL32)
 *
 * The target address is in EDX.
 * The 16 bit arguments start at ESP+4.
 * The number of 16bit argumentbytes is EBP-ESP-0x44 (68 Byte thunksetup).
 * [ok]
 */
VOID WIN32API QT_Thunk(CONTEXT *context)
{
    dprintf(("QT_Thunk\n"));
#if 0
    CONTEXT context16;
    DWORD   argsize;

    memcpy(&context16,context,sizeof(context16));

    CS_reg(&context16)   = HIWORD(EDX_reg(context));
    IP_reg(&context16)   = LOWORD(EDX_reg(context));

    argsize = EBP_reg(context)-ESP_reg(context)-0x44;

    /* additional 4 bytes used by the relaycode for storing the stackptr */
    memcpy( ((LPBYTE)CURRENT_STACK16)-argsize-4,
        (LPBYTE)ESP_reg(context)+4,
        argsize
    );
    EAX_reg(context) = CallTo16_regs_short(&context16,-argsize);
#endif
}
//******************************************************************************
/***********************************************************************
 * Generates a FT_Prolog call.
 *
 *  0FB6D1                  movzbl edx,cl
 *  8B1495xxxxxxxx      mov edx,[4*edx + xxxxxxxx]
 *  68xxxxxxxx          push FT_Prolog
 *  C3              lret
 */
static void _write_ftprolog(LPBYTE thunk,DWORD thunkstart)
{
 LPBYTE x;

  x = thunk;
  *x++  = 0x0f;*x++=0xb6;*x++=0xd1; /* movzbl edx,cl */
  *x++  = 0x8B;*x++=0x14;*x++=0x95;*(DWORD*)x= thunkstart;
  x+=4; /* mov edx, [4*edx + thunkstart] */
  *x++  = 0x68; *(DWORD*)x = (DWORD)FT_Prolog;
  x+=4;     /* push FT_Prolog */
  *x++  = 0xC3;     /* lret */
  /* fill rest with 0xCC / int 3 */
}
//******************************************************************************
/***********************************************************************
 * Generates a QT_Thunk style call.
 *
 *  33C9                    xor ecx, ecx
 *  8A4DFC                  mov cl , [ebp-04]
 *  8B148Dxxxxxxxx          mov edx, [4*ecx + (EAX+EDX)]
 *  B8yyyyyyyy              mov eax, QT_Thunk
 *  FFE0                    jmp eax
 */
static void _write_qtthunk(LPBYTE start,DWORD thunkstart)
{
 LPBYTE x;

  x = start;
  *x++  = 0x33;*x++=0xC9; /* xor ecx,ecx */
  *x++  = 0x8A;*x++=0x4D;*x++=0xFC; /* movb cl,[ebp-04] */
  *x++  = 0x8B;*x++=0x14;*x++=0x8D;*(DWORD*)x= thunkstart;
  x+=4; /* mov edx, [4*ecx + (EAX+EDX) */
  *x++  = 0xB8; *(DWORD*)x = (DWORD)QT_Thunk;
  x+=4;     /* mov eax , QT_Thunk */
  *x++  = 0xFF; *x++ = 0xE0;    /* jmp eax */
  /* should fill the rest of the 32 bytes with 0xCC */
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API ThunkConnect32(thunkstruct *ths,  LPSTR thunkfun16,
                                 LPSTR module16, LPSTR module32, HMODULE hmod32,
                                 DWORD dllinitarg1 )
{
 thunkstruct *ths16;

//  _interrupt(3);
  dprintf(("ThunkConnect32 %s %s %s not supported\n", thunkfun16, module16, module32));

  if(strncmp(ths->magic,"SL01",4) && strncmp(ths->magic,"LS01",4))
    return 0;

  ths16 = (thunkstruct *)LocalAlloc(LPTR, ths->length);
  ths16->length = ths->length;
  ths16->ptr    = (DWORD)ThunkProc;

  if(!strncmp(ths->magic,"SL01",4))  {
    ths->x0C = (DWORD)ths16;
    *(DWORD *)ths16->magic  = 0x0000304C;
  }
  if(!strncmp(ths->magic,"LS01",4))  {
    ths->ptr = ths16->ptr;
    /* code offset for QT_Thunk is at 0x1C...  */
    _write_qtthunk (((LPBYTE)ths) + ths->x1C,ths->ptr);

    /* code offset for FT_Prolog is at 0x20...  */
    _write_ftprolog(((LPBYTE)ths) + ths->x20,ths->ptr);
    return 1;
  }
  return TRUE;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API K32Thk1632Prolog(DWORD arg1)
{
  dprintf(("OS2K32Thk1632Prolog %X not supported\n", arg1));
  return(0);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API K32Thk1632Epilog(DWORD arg1)
{
  dprintf(("K32Thk1632Epilog %X not supported\n", arg1));
  return(0);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API MapSLFix(DWORD arg1)
{
  dprintf(("MapSLFix %X not supported\n", arg1));
  return(0);
}
//******************************************************************************
//******************************************************************************
