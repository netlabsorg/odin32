; $Id: mytkStartProcess.asm,v 1.1.2.1 2000-08-21 22:59:40 bird Exp $
;
; tkStartProcess overloader. Needed to clear the loader semaphore
; when a process is being started syncronously.
;
; Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;

    .386p


;
;   Include files
;
    include devsegdf.inc

;
;   Imported Functions and variables.
;
    ;
    ; LDR semaphore
    ;
    extrn  pLDRSem:DWORD
    extrn  _KSEMRequestMutex@8:PROC
    extrn  _LDRClearSem@0:PROC

    ;
    ; Loader State
    ;
    extrn ulLDRState:DWORD

    ;
    ;
    ;
    extrn fLdrSemTaken:BYTE
    extrn fTkExecPgm:BYTE

    ;
    ; Pointer to current executable module.
    ;
    extrn pExeModule:DWORD

    ;
    ;   Exported symbols
    ;
    public mytkStartProcess


CODE32 SEGMENT
mytkStartProcess PROC NEAR
    ASSUME CS:CODE32, DS:FLAT, ES:NOTHING, SS:NOTHING

    ;
    ; Check if the loader semphore is take.
    ; Clear it (and other vars) if it is.
    ;
    cmp     fLdrSemTaken, 0
    jz      finished

    mov     ulLDRState, 0               ; Clears loaderstate. (LDRSTATE_UNKNOWN)
    mov     pExeModule, 0               ; Sets the exemodule pointer to NULL.
    mov     fTkExecPgm, 0               ; Marks global data invalid.
    call    near ptr FLAT:_LDRClearSem@0
    mov     fLdrSemTaken, 0             ; Loader semaphore is not taken any longer!

finished:
    call    ktStartProcess
    ret
mytkStartProcess ENDP

CODE32 ENDS
END
