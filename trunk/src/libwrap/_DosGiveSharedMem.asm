        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosGiveSharedMem:PROC
        PUBLIC  _DosGiveSharedMem
_DosGiveSharedMem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosGiveSharedMem
    add    esp, 12
    pop    fs
    ret
_DosGiveSharedMem ENDP

CODE32          ENDS

                END
