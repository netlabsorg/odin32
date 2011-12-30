        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosCloseEventSem:PROC
        PUBLIC  _DosCloseEventSem
_DosCloseEventSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosCloseEventSem
    add    esp, 4
    pop    fs
    ret
_DosCloseEventSem ENDP

CODE32          ENDS

                END
