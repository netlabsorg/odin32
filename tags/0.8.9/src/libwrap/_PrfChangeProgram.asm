        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   PrfChangeProgram:PROC
        PUBLIC  _PrfChangeProgram
_PrfChangeProgram PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   PrfChangeProgram
    add    esp, 12
    pop    fs
    ret
_PrfChangeProgram ENDP

CODE32          ENDS

                END
