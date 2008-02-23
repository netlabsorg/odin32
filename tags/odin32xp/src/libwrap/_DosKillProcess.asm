        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosKillProcess:PROC
        PUBLIC  _DosKillProcess
_DosKillProcess PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosKillProcess
    add    esp, 8
    pop    fs
    ret
_DosKillProcess ENDP

CODE32          ENDS

                END
