        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   PrfQueryProfileInt:PROC
        PUBLIC  _PrfQueryProfileInt
_PrfQueryProfileInt PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   PrfQueryProfileInt
    add    esp, 16
    pop    fs
    ret
_PrfQueryProfileInt ENDP

CODE32          ENDS

                END
