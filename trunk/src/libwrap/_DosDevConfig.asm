        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosDevConfig:PROC
        PUBLIC  _DosDevConfig
_DosDevConfig PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosDevConfig
    add    esp, 8
    pop    fs
    ret
_DosDevConfig ENDP

CODE32          ENDS

                END
