        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosGetNamedSharedMem:PROC
        PUBLIC  _DosGetNamedSharedMem
_DosGetNamedSharedMem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosGetNamedSharedMem
    add    esp, 12
    pop    fs
    ret
_DosGetNamedSharedMem ENDP

CODE32          ENDS

                END
