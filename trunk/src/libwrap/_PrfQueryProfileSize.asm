        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   PrfQueryProfileSize:PROC
        PUBLIC  _PrfQueryProfileSize
_PrfQueryProfileSize PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   PrfQueryProfileSize
    add    esp, 16
    pop    fs
    ret
_PrfQueryProfileSize ENDP

CODE32          ENDS

                END
