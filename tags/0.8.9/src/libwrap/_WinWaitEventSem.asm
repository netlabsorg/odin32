        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinWaitEventSem:PROC
        PUBLIC  _WinWaitEventSem
_WinWaitEventSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinWaitEventSem
    add    esp, 8
    pop    fs
    ret
_WinWaitEventSem ENDP

CODE32          ENDS

                END
