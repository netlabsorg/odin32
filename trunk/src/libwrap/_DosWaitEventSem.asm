        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosWaitEventSem:PROC
        PUBLIC  _DosWaitEventSem
_DosWaitEventSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosWaitEventSem
    add    esp, 8
    pop    fs
    ret
_DosWaitEventSem ENDP

CODE32          ENDS

                END
