        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   PrfCreateGroup:PROC
        PUBLIC  _PrfCreateGroup
_PrfCreateGroup PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   PrfCreateGroup
    add    esp, 12
    pop    fs
    ret
_PrfCreateGroup ENDP

CODE32          ENDS

                END
