        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosRequestMutexSem:PROC
        PUBLIC  _DosRequestMutexSem
_DosRequestMutexSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosRequestMutexSem
    add    esp, 8
    pop    fs
    ret
_DosRequestMutexSem ENDP

CODE32          ENDS

                END
