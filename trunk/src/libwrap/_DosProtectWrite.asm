        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosProtectWrite:PROC
        PUBLIC  _DosProtectWrite
_DosProtectWrite PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   DosProtectWrite
    add    esp, 20
    pop    fs
    ret
_DosProtectWrite ENDP

CODE32          ENDS

                END
