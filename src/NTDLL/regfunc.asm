; $Id: regfunc.asm,v 1.5 2003-04-07 18:40:51 sandervl Exp $

;/*
; * register functions in NTDLL
; *
; * Copyright 1999 Patrick Haller
; *
; *
; * Project Odin Software License can be found in LICENSE.TXT
; *
; */
.586P
               NAME    regfunc

CODE32         SEGMENT DWORD PUBLIC USE32 'CODE'
               align 4

; ----------------------------------------------------------------------------

       public  _chkstk      
       public  _alloca_probe; _alloca_probe seems to be the same as _chkstk
       public  DbgBreakPoint


; ----------------------------------------------------------------------------


; ----------------------------------------------------------------------------


; ----------------------------------------------------------------------------
; Name      : _chkstk
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : 
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_chkstk proc near

arg_0          = byte ptr  8

               push    ecx             ; _alloca_probe
               cmp     eax, 1000h
               lea     ecx, [esp+arg_0]
               jb      short _chkstk_1

_chkstk_2:
               sub     ecx, 1000h
               sub     eax, 1000h
               test    [ecx], eax
               cmp     eax, 1000h
               jnb     short _chkstk_2

_chkstk_1:
               sub     ecx, eax
               mov     eax, esp
               test    [ecx], eax
               mov     esp, ecx
               mov     ecx, [eax]
               mov     eax, [eax+4]
               push    eax
               retn
_chkstk        endp


; ----------------------------------------------------------------------------
; Name      : _alloca_probe
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : 
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_alloca_probe proc near

arg_0          = byte ptr  8

               push    ecx             ; _alloca_probe
               cmp     eax, 1000h
               lea     ecx, [esp+arg_0]
               jb      short _alloca_probe_1

_alloca_probe_2:
               sub     ecx, 1000h
               sub     eax, 1000h
               test    [ecx], eax
               cmp     eax, 1000h
               jnb     short _alloca_probe_2

_alloca_probe_1:
               sub     ecx, eax
               mov     eax, esp
               test    [ecx], eax
               mov     esp, ecx
               mov     ecx, [eax]
               mov     eax, [eax+4]
               push    eax
               retn
_alloca_probe  endp


; ----------------------------------------------------------------------------
; Name      : DbgBreakPoint
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.20
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
DbgBreakPoint  proc near
               int     3               ; Trap to Debugger
               retn
DbgBreakPoint  endp


; ----------------------------------------------------------------------------
; Name      : DbgUserBreakPoint
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.28
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
DbgUserBreakPoint proc near
               int     3               ; Trap to Debugger
               retn
DbgUserBreakPoint endp


; ----------------------------------------------------------------------------
CODE32          ENDS

                END
