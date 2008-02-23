        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosReleaseMutexSem:PROC
        PUBLIC  _DosReleaseMutexSem
_DosReleaseMutexSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosReleaseMutexSem
    add    esp, 4
    pop    fs
    ret
_DosReleaseMutexSem ENDP

CODE32          ENDS

                END
