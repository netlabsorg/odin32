        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosOpenMutexSem:PROC
        PUBLIC  _DosOpenMutexSem
_DosOpenMutexSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosOpenMutexSem
    add    esp, 8
    pop    fs
    ret
_DosOpenMutexSem ENDP

CODE32          ENDS

                END
