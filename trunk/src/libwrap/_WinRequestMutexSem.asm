        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinRequestMutexSem:PROC
        PUBLIC  _WinRequestMutexSem
_WinRequestMutexSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinRequestMutexSem
    add    esp, 8
    pop    fs
    ret
_WinRequestMutexSem ENDP

CODE32          ENDS

                END
