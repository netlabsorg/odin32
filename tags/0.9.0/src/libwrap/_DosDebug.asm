        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosDebug:PROC
        PUBLIC  _DosDebug
_DosDebug PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosDebug
    add    esp, 4
    pop    fs
    ret
_DosDebug ENDP

CODE32          ENDS

                END
