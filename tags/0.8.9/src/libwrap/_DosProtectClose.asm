        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosProtectClose:PROC
        PUBLIC  _DosProtectClose
_DosProtectClose PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosProtectClose
    add    esp, 8
    pop    fs
    ret
_DosProtectClose ENDP

CODE32          ENDS

                END
