        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosKillThread:PROC
        PUBLIC  _DosKillThread
_DosKillThread PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosKillThread
    add    esp, 4
    pop    fs
    ret
_DosKillThread ENDP

CODE32          ENDS

                END
