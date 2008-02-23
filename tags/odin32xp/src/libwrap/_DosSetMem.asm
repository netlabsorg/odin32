        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSetMem:PROC
        PUBLIC  _DosSetMem
_DosSetMem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosSetMem
    add    esp, 12
    pop    fs
    ret
_DosSetMem ENDP

CODE32          ENDS

                END
