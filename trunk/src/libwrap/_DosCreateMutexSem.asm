        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosCreateMutexSem:PROC
        PUBLIC  _DosCreateMutexSem
_DosCreateMutexSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosCreateMutexSem
    add    esp, 16
    pop    fs
    ret
_DosCreateMutexSem ENDP

CODE32          ENDS

                END
