        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosProtectSetFileLocks:PROC
        PUBLIC  _DosProtectSetFileLocks
_DosProtectSetFileLocks PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   DosProtectSetFileLocks
    add    esp, 24
    pop    fs
    ret
_DosProtectSetFileLocks ENDP

CODE32          ENDS

                END
