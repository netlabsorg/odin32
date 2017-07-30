        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   PrfQueryProgramCategory:PROC
        PUBLIC  _PrfQueryProgramCategory
_PrfQueryProgramCategory PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   PrfQueryProgramCategory
    add    esp, 8
    pop    fs
    ret
_PrfQueryProgramCategory ENDP

CODE32          ENDS

                END
