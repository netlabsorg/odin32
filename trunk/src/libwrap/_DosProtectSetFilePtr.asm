        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosProtectSetFilePtr:PROC
        PUBLIC  _DosProtectSetFilePtr
_DosProtectSetFilePtr PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   DosProtectSetFilePtr
    add    esp, 20
    pop    fs
    ret
_DosProtectSetFilePtr ENDP

CODE32          ENDS

                END
