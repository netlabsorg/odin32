/* $Id: thunk.cpp,v 1.6 2001-03-13 18:45:34 sandervl Exp $ */

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
 * Copyright 1995 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <string.h>
#include "thunk.h"

#define DBG_LOCALLOG	DBG_thunk
#include "dbglocal.h"

/************************************************************************
 *           _ConfirmWin16Lock    (KERNEL32.96)
 */
DWORD WINAPI _ConfirmWin16Lock(void)
{
	dprintf(("STUB: _ConfirmWin16Lock"));
        return 0;
}

/************************************************************************
 *           ReleaseThunkLock    (KERNEL32.48)
 */
VOID WINAPI ReleaseThunkLock(DWORD *mutex_count)
{
	dprintf(("STUB: ReleaseThunkLock"));
}

/************************************************************************
 *           RestoreThunkLock    (KERNEL32.49)
 */
VOID WINAPI RestoreThunkLock(DWORD mutex_count)
{
	dprintf(("STUB: RestoreThunkLock"));
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API MapLS(void *address)
{
//  _interrupt(3);
  dprintf(("MapLS %X, not supported\n", address));
  return((DWORD)address);
}
/***********************************************************************
 *           UnMapLS   (KERNEL32.700)
 *
 * Free mapped selector.
 */
void WIN32API UnMapLS( SEGPTR sptr )
{
  dprintf(("UnMapLS - stub\n"));
#if 0
    if (SELECTOROF(sptr))
        SELECTOR_FreeBlock( SELECTOROF(sptr), 1 );
#endif
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
/**********************************************************************
 * 		FT_Thunk			(KERNEL32.234)
 *
 * This routine performs the actual call to 16-bit code,
 * similar to QT_Thunk. The differences are:
 *  - The call target is taken from the buffer created by FT_Prolog
 *  - Those arguments requested by the thunk code (by setting the
 *    corresponding bit in the bitmap at EBP-20) are converted
 *    from 32-bit pointers to segmented pointers (those pointers
 *    are guaranteed to point to structures copied to the stack
 *    by the thunk code, so we always use the 16-bit stack selector
 *    for those addresses).
 *
 *    The bit #i of EBP-20 corresponds here to the DWORD starting at
 *    ESP+4 + 2*i.
 *
 * FIXME: It is unclear what happens if there are more than 32 WORDs
 *        of arguments, so that the single DWORD bitmap is no longer
 *        sufficient ...
 */
void WINAPI FT_Thunk( CONTEXT86 *context )
{
    DWORD mapESPrelative = *(DWORD *)(EBP_reg(context) - 20);
    DWORD callTarget     = *(DWORD *)(EBP_reg(context) - 52);

    CONTEXT86 context16;
    DWORD i, argsize;
    LPBYTE newstack, oldstack;

    dprintf(("FT_Thunk - stub\n"));

#if 0
    memcpy(&context16,context,sizeof(context16));

    CS_reg(&context16)  = HIWORD(callTarget);
    EIP_reg(&context16) = LOWORD(callTarget);
    EBP_reg(&context16) = OFFSETOF( NtCurrentTeb()->cur_stack )
                           + (WORD)&((STACK16FRAME*)0)->bp;

    argsize  = EBP_reg(context)-ESP_reg(context)-0x40;
    newstack = (LPBYTE)CURRENT_STACK16 - argsize;
    oldstack = (LPBYTE)ESP_reg(context);

    memcpy( newstack, oldstack, argsize );

    for (i = 0; i < 32; i++)	/* NOTE: What about > 32 arguments? */
	if (mapESPrelative & (1 << i))
	{
	    SEGPTR *arg = (SEGPTR *)(newstack + 2*i);
	    *arg = PTR_SEG_OFF_TO_SEGPTR(SELECTOROF(NtCurrentTeb()->cur_stack),
                                         OFFSETOF(NtCurrentTeb()->cur_stack) - argsize
					 + (*(LPBYTE *)arg - oldstack));
	}

    EAX_reg(context) = Callbacks->CallRegisterShortProc( &context16, argsize );
    EDX_reg(context) = HIWORD(EAX_reg(context));
    EAX_reg(context) = LOWORD(EAX_reg(context));

    /* Copy modified buffers back to 32-bit stack */
    memcpy( oldstack, newstack, argsize );
#endif
}

/**********************************************************************
 * 		FT_ExitNN		(KERNEL32.218 - 232)
 *
 * One of the FT_ExitNN functions is called at the end of the thunk code.
 * It removes the stack frame created by FT_Prolog, moves the function
 * return from EBX to EAX (yes, FT_Thunk did use EAX for the return
 * value, but the thunk code has moved it from EAX to EBX in the
 * meantime ... :-), restores the caller's EBX, ESI, and EDI registers,
 * and perform a return to the CALLER of the thunk code (while removing
 * the given number of arguments from the caller's stack).
 */

static void FT_Exit(CONTEXT86 *context, int nPopArgs)
{
    dprintf(("FT_Exit - stub\n"));
#if 0
    /* Return value is in EBX */
    EAX_reg(context) = EBX_reg(context);

    /* Restore EBX, ESI, and EDI registers */
    EBX_reg(context) = *(DWORD *)(EBP_reg(context) -  4);
    ESI_reg(context) = *(DWORD *)(EBP_reg(context) -  8);
    EDI_reg(context) = *(DWORD *)(EBP_reg(context) - 12);

    /* Clean up stack frame */
    ESP_reg(context) = EBP_reg(context);
    EBP_reg(context) = stack32_pop(context);

    /* Pop return address to CALLER of thunk code */
    EIP_reg(context) = stack32_pop(context);
    /* Remove arguments */
    ESP_reg(context) += nPopArgs;
#endif
}

void WINAPI FT_Exit0 (CONTEXT86 *context) { FT_Exit(context,  0); }
void WINAPI FT_Exit4 (CONTEXT86 *context) { FT_Exit(context,  4); }
void WINAPI FT_Exit8 (CONTEXT86 *context) { FT_Exit(context,  8); }
void WINAPI FT_Exit12(CONTEXT86 *context) { FT_Exit(context, 12); }
void WINAPI FT_Exit16(CONTEXT86 *context) { FT_Exit(context, 16); }
void WINAPI FT_Exit20(CONTEXT86 *context) { FT_Exit(context, 20); }
void WINAPI FT_Exit24(CONTEXT86 *context) { FT_Exit(context, 24); }
void WINAPI FT_Exit28(CONTEXT86 *context) { FT_Exit(context, 28); }
void WINAPI FT_Exit32(CONTEXT86 *context) { FT_Exit(context, 32); }
void WINAPI FT_Exit36(CONTEXT86 *context) { FT_Exit(context, 36); }
void WINAPI FT_Exit40(CONTEXT86 *context) { FT_Exit(context, 40); }
void WINAPI FT_Exit44(CONTEXT86 *context) { FT_Exit(context, 44); }
void WINAPI FT_Exit48(CONTEXT86 *context) { FT_Exit(context, 48); }
void WINAPI FT_Exit52(CONTEXT86 *context) { FT_Exit(context, 52); }
void WINAPI FT_Exit56(CONTEXT86 *context) { FT_Exit(context, 56); }
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
/***********************************************************************
 *           UnMapSLFixArray   (KERNEL32.701)
 */
void WIN32API UnMapSLFixArray( SEGPTR sptr[], INT length, CONTEXT86 *context )
{
    /* Must not change EAX, hence defined as 'register' function */
}
/**********************************************************************
 * 		SMapLS*		(KERNEL32)
 * These functions map linear pointers at [EBP+xxx] to segmented pointers
 * and return them.
 * Win95 uses some kind of alias structs, which it stores in [EBP+x] to
 * unravel them at SUnMapLS. We just store the segmented pointer there.
 */
static void
x_SMapLS_IP_EBP_x(CONTEXT86 *context,int argoff) {
    DWORD	val,ptr;

    val =*(DWORD*)(EBP_reg(context)+argoff);
    if (val<0x10000) {
	ptr=val;
	*(DWORD*)(EBP_reg(context)+argoff) = 0;
    } else {
    	ptr = MapLS((LPVOID)val);
	*(DWORD*)(EBP_reg(context)+argoff) = ptr;
    }
    EAX_reg(context) = ptr;
}

void WINAPI SMapLS_IP_EBP_8 (CONTEXT86 *context) {x_SMapLS_IP_EBP_x(context, 8);}
void WINAPI SMapLS_IP_EBP_12(CONTEXT86 *context) {x_SMapLS_IP_EBP_x(context,12);}
void WINAPI SMapLS_IP_EBP_16(CONTEXT86 *context) {x_SMapLS_IP_EBP_x(context,16);}
void WINAPI SMapLS_IP_EBP_20(CONTEXT86 *context) {x_SMapLS_IP_EBP_x(context,20);}
void WINAPI SMapLS_IP_EBP_24(CONTEXT86 *context) {x_SMapLS_IP_EBP_x(context,24);}
void WINAPI SMapLS_IP_EBP_28(CONTEXT86 *context) {x_SMapLS_IP_EBP_x(context,28);}
void WINAPI SMapLS_IP_EBP_32(CONTEXT86 *context) {x_SMapLS_IP_EBP_x(context,32);}
void WINAPI SMapLS_IP_EBP_36(CONTEXT86 *context) {x_SMapLS_IP_EBP_x(context,36);}
void WINAPI SMapLS_IP_EBP_40(CONTEXT86 *context) {x_SMapLS_IP_EBP_x(context,40);}

void WINAPI SMapLS( CONTEXT86 *context )
{
    if (EAX_reg(context)>=0x10000) {
	EAX_reg(context) = MapLS((LPVOID)EAX_reg(context));
	EDX_reg(context) = EAX_reg(context);
    } else {
	EDX_reg(context) = 0;
    }
}

void WINAPI SUnMapLS( CONTEXT86 *context )
{
    if (EAX_reg(context)>=0x10000)
	UnMapLS((SEGPTR)EAX_reg(context));
}

static void
x_SUnMapLS_IP_EBP_x(CONTEXT86 *context,int argoff) {
	if (*(DWORD*)(EBP_reg(context)+argoff))
		UnMapLS(*(DWORD*)(EBP_reg(context)+argoff));
	*(DWORD*)(EBP_reg(context)+argoff)=0;
}
void WINAPI SUnMapLS_IP_EBP_8 (CONTEXT86 *context) { x_SUnMapLS_IP_EBP_x(context, 8); }
void WINAPI SUnMapLS_IP_EBP_12(CONTEXT86 *context) { x_SUnMapLS_IP_EBP_x(context,12); }
void WINAPI SUnMapLS_IP_EBP_16(CONTEXT86 *context) { x_SUnMapLS_IP_EBP_x(context,16); }
void WINAPI SUnMapLS_IP_EBP_20(CONTEXT86 *context) { x_SUnMapLS_IP_EBP_x(context,20); }
void WINAPI SUnMapLS_IP_EBP_24(CONTEXT86 *context) { x_SUnMapLS_IP_EBP_x(context,24); }
void WINAPI SUnMapLS_IP_EBP_28(CONTEXT86 *context) { x_SUnMapLS_IP_EBP_x(context,28); }
void WINAPI SUnMapLS_IP_EBP_32(CONTEXT86 *context) { x_SUnMapLS_IP_EBP_x(context,32); }
void WINAPI SUnMapLS_IP_EBP_36(CONTEXT86 *context) { x_SUnMapLS_IP_EBP_x(context,36); }
void WINAPI SUnMapLS_IP_EBP_40(CONTEXT86 *context) { x_SUnMapLS_IP_EBP_x(context,40); }

