        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSetFileLocks:PROC
        PUBLIC  _DosSetFileLocks
_DosSetFileLocks PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   DosSetFileLocks
    add    esp, 20
    pop    fs
    ret
_DosSetFileLocks ENDP

CODE32          ENDS

                END
