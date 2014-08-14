        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosProtectRead:PROC
        PUBLIC  _DosProtectRead
_DosProtectRead PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   DosProtectRead
    add    esp, 20
    pop    fs
    ret
_DosProtectRead ENDP

CODE32          ENDS

                END
