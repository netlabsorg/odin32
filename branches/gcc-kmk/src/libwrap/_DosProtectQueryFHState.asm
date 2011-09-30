        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosProtectQueryFHState:PROC
        PUBLIC  _DosProtectQueryFHState
_DosProtectQueryFHState PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosProtectQueryFHState
    add    esp, 12
    pop    fs
    ret
_DosProtectQueryFHState ENDP

CODE32          ENDS

                END
