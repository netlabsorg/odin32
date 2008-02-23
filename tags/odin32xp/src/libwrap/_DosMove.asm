        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosMove:PROC
        PUBLIC  _DosMove
_DosMove PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosMove
    add    esp, 8
    pop    fs
    ret
_DosMove ENDP

CODE32          ENDS

                END
