        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryMutexSem:PROC
        PUBLIC  _DosQueryMutexSem
_DosQueryMutexSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosQueryMutexSem
    add    esp, 16
    pop    fs
    ret
_DosQueryMutexSem ENDP

CODE32          ENDS

                END
