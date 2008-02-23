        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   PrfQueryProfile:PROC
        PUBLIC  _PrfQueryProfile
_PrfQueryProfile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   PrfQueryProfile
    add    esp, 8
    pop    fs
    ret
_PrfQueryProfile ENDP

CODE32          ENDS

                END
