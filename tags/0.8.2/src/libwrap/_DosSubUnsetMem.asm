        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSubUnsetMem:PROC
        PUBLIC  _DosSubUnsetMem
_DosSubUnsetMem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosSubUnsetMem
    add    esp, 4
    pop    fs
    ret
_DosSubUnsetMem ENDP

CODE32          ENDS

                END
