; $Id: libCallThruCallGate.asm,v 1.1.2.1 2002-04-01 13:04:54 bird Exp $
;
; Calls Win32k thru the call gate.
;
; Copyright (c) 2001-2002 knut st. osmundsen (bird@anduin.net)
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p

;
; Exported symbols
;
    public libCallThruCallGate
    public usCGSelector


DATA32 segment
f48CallGate:
    ulOffset        dd 0
    usCGSelector    dw 0
DATA32 ends


CODE32 segment
    assume CS:CODE32, DS:FLAT, SS:NOTHING
;;
;
; @cproto    APIRET APIENTRY libWin32kCallThruCallGate(ULONG ulFunction, PVOID pvParam);
; @returns   CS value.
; @author    knut st. osmundsen (bird@anduin.net)
libCallThruCallGate proc near
    ASSUME  ds:FLAT, ss:FLAT
    push    dword ptr [esp+08h]
    push    dword ptr [esp+08h]
    call    fword ptr ds:[ulOffset]
    add     esp, 8
    ret
libCallThruCallGate endp

CODE32 ends
end