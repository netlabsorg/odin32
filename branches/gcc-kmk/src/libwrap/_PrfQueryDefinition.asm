        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   PrfQueryDefinition:PROC
        PUBLIC  _PrfQueryDefinition
_PrfQueryDefinition PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   PrfQueryDefinition
    add    esp, 16
    pop    fs
    ret
_PrfQueryDefinition ENDP

CODE32          ENDS

                END
