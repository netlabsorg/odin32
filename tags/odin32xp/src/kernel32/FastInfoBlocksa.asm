; $Id: FastInfoBlocksa.asm,v 1.1 2003-03-06 10:22:25 sandervl Exp $
;
; Fast InfoBlock Access.
;
; Copyright (c) 2003 knut st. osmundsen <bird@anduin.net>
;
; Project Odin Software License can be found in LICENSE.TXT.
;
;


.386

CODE32 segment use32 dword public 'CODE'
CODE32 ends

CODE16 segment use16 dword public 'CODE'
CODE16 ends

DATA32 segment use32 dword public 'DATA'
DATA32 ends


;*******************************************************************************
;* Global Variables                                                            *
;*******************************************************************************
DATA32 segment use32 dword public 'DATA'
_gpfibLIS       dd 0
_gfpfibLIS      dd 0
_gpfibGIS       dd 0
_gfpfibGIS      dd 0
_gpfibPIB       dd 0
DATA32 ends

extrn fibDumpAll:near


;*******************************************************************************
;* Exported Symbols                                                            *
;*******************************************************************************
public _gpfibLIS
public _gfpfibLIS
public _gpfibGIS
public _gfpfibGIS
public _gpfibPIB
public fibInit


CODE32 segment
    ASSUME ds:FLAT, es:FLAT, ss:FLAT, fs:nothing, gs:nothing

;;
; Initiates the globals of the fib.
;
; @cproto   int     fibInit(void);
; @returns  0 on success
; @returns  OS/2 error code on failure
; @uses     eax, edx, ecx
; @sketch
; @status
; @author   knut st. osmundsen<bird@anduin.net>
; @remark
;
fibInit proc near
    push    ebp
    mov     ebp, esp
    cmp     _gpfibPIB, 0
    jz      @doinit
    jmp     @exit_ok
@doinit:

if 1
    ;
    ; Ensure stack 512bytes of stack.
    ;
    cmp     sp, 0f800h
    jb @stackok
    sub     esp, 800h
    mov     eax, [esp]                  ; touch it
@stackok:

    ;
    ; Now call  APIRET APIENTRY DosGetInfoSeg(PSEL pselGlobal, PSEL pselLocal);
    ; In order to do this, we must have a 16-bit stack.
    ;
    ; prepare switch back.
    mov     edx, esp
    push    ss
    push    edx
    ; the switch.
    mov     eax, esp
    shr     eax, 13
    and     eax, 0fff8h
    or      eax, 7h
    mov     edx, esp
    and     edx, 0ffffh
    push    eax
    push    edx
    lss     esp, ss:[esp]
    ASSUME ss:nothing

    ; Now we're on 16-bit stack.
    ; Lets go to 16-bit code too
    jmp     far ptr CODE16:Thunk16_fibInit
CODE32 ends
CODE16 segment
Thunk16_fibInit::
    push    0
    mov     cx, sp
    push    0
    mov     dx, sp
    push    ss
    push    cx
    push    ss
    push    dx
    extrn DOS16GETINFOSEG:far
    call    DOS16GETINFOSEG
    pop     dx                          ; sel LIS
    pop     cx                          ; sel GIS
    jmp far ptr FLAT:Thunk32_fibInit
CODE16 ends
CODE32 segment
Thunk32_fibInit::
    ; Switch back the stack
    lss     esp, ss:[esp]
    ASSUME ss:FLAT

    ;
    ; Check return value.
    ;
    movsx   eax, ax
    or      eax, eax
    jnz     @exit

else

    ;
    ; Use the DOSCALLS imports
    ;
    extrn DOSGLOBALINFO:abs
    extrn DOSLOCALINFO:abs
    mov     edx, DOSLOCALINFO
    mov     ecx, DOSGLOBALINFO

endif

    ;
    ; Store far pointers.
    ;
    mov     word ptr [_gfpfibLIS + 2], dx
    mov     word ptr [_gfpfibGIS + 2], cx

    ;
    ; Make 32-bit pointers of the return values.
    ;
    and     edx, 0fff8h
    shl     edx, 13
    mov     [_gpfibLIS], edx

    and     ecx, 0fff8h
    shl     ecx, 13
    mov     [_gpfibGIS], ecx


    ;
    ; Call APIRET APIENTRY DosGetInfoBlocks(PTIB *pptib,PPIB *pppib);
    ;
    push    0
    mov     edx, esp
    push    0
    mov     eax, esp
    push    edx
    push    eax
    mov     al, 2
    extrn DosGetInfoBlocks:near
    call    DosGetInfoBlocks            ; assumes that this *never* failes.
    add     esp, 8h
    pop     eax                         ; ptib, ignore.
    pop     eax                         ; ppib
    mov     [_gpfibPIB], eax

    ;
    ; Return to caller.
    ;
@exit_ok:
    xor     eax,eax
@exit:
    leave
    ret
fibInit endp


CODE32 ends


end
