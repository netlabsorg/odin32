        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   PrfQueryProgramHandle:PROC
        PUBLIC  _PrfQueryProgramHandle
_PrfQueryProgramHandle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   PrfQueryProgramHandle
    add    esp, 20
    pop    fs
    ret
_PrfQueryProgramHandle ENDP

CODE32          ENDS

                END
