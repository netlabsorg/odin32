; $Id: mytkStartProcess.asm,v 1.1.2.4 2000-08-24 01:36:26 bird Exp $
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
    extrn pLdrSem:DWORD
    extrn _LDRClearSem@0:PROC
    extrn _KSEMRequestMutex@8:PROC
    extrn _KSEMQueryMutex@8:PROC

    ;
    ; Loader State
    ;
    extrn ulLDRState:DWORD

    ;
    ; mytkExecPgm stuff.
    ;
    extrn fTkExecPgm:BYTE

    ;
    ; Pointer to current executable module.
    ;
    extrn pExeModule:DWORD

    ;
    ; TKSSBase (32-bit)
    ;
    extrn pulTKSSBase32:DWORD

    ;
    ; Calltable entry for tkStartProcess
    ;
    extrn _tkStartProcess:PROC

;
;   Exported symbols
;
    public mytkStartProcess


CODE32 SEGMENT

;;
; Overloads tkStartProcess. If the loader semahore is taken by us we'll release it.
; @returns
; @param
; @uses      eax, ecx, edx
; @sketch
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
mytkStartProcess PROC NEAR
    ASSUME DS:FLAT, ES:NOTHING, SS:NOTHING

    ;
    ; Check if the loader semphore is take by us.
    ; Clear it (and other vars) if it is.
    ;
    push    eax
    push    ecx
    push    edx

    push    0                           ; Usage count variable.
    mov     eax, pulTKSSBase32          ; Get TKSSBase
    mov     eax, [eax]
    add     eax, esp                    ; Added TKSSBase to the usage count pointer
    push    eax                         ; Push address of usage count pointer.
    push    pLdrSem                     ; Push pointer to loader semaphore ( = handle).
    call    near ptr FLAT:_KSEMQueryMutex@8
    or      eax, eax                    ; Check return code. (1 = our / free; 0 = not our but take)
    pop     eax                         ; Pops usage count.
    jz      mtksp_ret                   ; jmp if not taken by us (rc=FALSE).
    or      eax, eax                    ; Check usage count.
    jz      mtksp_ret                   ; jmp if 0 (=free).
    mov     ulLDRState, 0               ; Clears loaderstate. (LDRSTATE_UNKNOWN)
    mov     pExeModule, 0               ; Sets the exemodule pointer to NULL.
    mov     fTkExecPgm, 0               ; Marks global data invalid.
    cmp     eax, 1                      ; If usage count > 1 then issue a release. (should not happen here)
    je      mtksp_ret                   ; jmp if usage count < 2. (tkStartProcess or tkExecPgm will free the last usage).
    call    near ptr FLAT:_LDRClearSem@0

mtksp_ret:
    pop     edx
    pop     ecx
    pop     eax
    jmp     near ptr FLAT:_tkStartProcess
mytkStartProcess ENDP

CODE32 ENDS
END
