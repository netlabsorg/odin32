; $Id: kdtStrategy.asm,v 1.1 2002-09-30 23:53:54 bird Exp $
;
; Ring-3 Device Testing: Strategy
;
;
; Copyright (c) 2002 knut st. osmundsen <bird@anduin.net>
;
;
; This file is part of kKrnlLib.
;
; kKrnlLib is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; kKrnlLib is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with kKrnlLib; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
;

    .386


;*******************************************************************************
;* Header Files                                                                *
;*******************************************************************************
include kdtSegDef.inc
include devhlp.inc


;*******************************************************************************
;* Externs                                                                     *
;*******************************************************************************
CODE32 segment
extrn kdtStackThunk32To16:NEAR
extrn kdtStackThunk16To32:NEAR
CODE32 ends



CODE32 segment

;;
;
; @cproto   BOOL kdtStrategySend(ULONG fpfnStrat, ULONG fpRP, USHORT usDS);
; @returns  Success indicator.
; @param    fpfnStrat   Far pointer to the strategy routine. (eax)
; @param    fpRP        Far pointer to the request packet. (edx)
; @param    usDS        Data segment of the device driver. (cx)
; @uses     eax, edx, ecx
; @status   completely implemented.
; @author   knut st. osmundsen<bird@anduin.net>
; @remark   Called on 32-bit stack!
;
kdtStrategySend proc near
    push    ebp
    mov     ebp, esp
    push    eax
    push    es
    push    ds
    push    ebx


    ;
    ; Thunk stack to 16-bit
    ;
    push    edx
    push    ecx
    call    kdtStackThunk32To16
    pop     ecx
    pop     edx


    ;
    ; Call the thunker
    ;
    jmp     far ptr CODE16:kdtStrategySendThunk16
CODE32 ends
CODE16 segment
kdtStrategySendThunk16::
    mov     ds, cx
    mov     bx, dx
    shr     edx, 10h
    mov     es, dx
;    xor     edx, edx
    xor     ecx, ecx
    xor     eax, eax
    call    dword ptr ss:[bp-4]
    jmp     far ptr FLAT:kdtStrategySendThunk32
CODE16 ends
CODE32 segment
kdtStrategySendThunk32::


    ;
    ; Thunk stack to 32-bit
    ;
    push    eax
    call    kdtStackThunk16To32
    pop     eax

    ; cleanup and return.
    pop     ebx
    pop     ds
    pop     es
    leave
    mov     eax, 1                      ; success
    ret
kdtStrategySend endp



CODE32 ends


end

