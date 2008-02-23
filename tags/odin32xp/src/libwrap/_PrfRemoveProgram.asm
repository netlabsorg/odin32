        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   PrfRemoveProgram:PROC
        PUBLIC  _PrfRemoveProgram
_PrfRemoveProgram PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   PrfRemoveProgram
    add    esp, 8
    pop    fs
    ret
_PrfRemoveProgram ENDP

CODE32          ENDS

                END
