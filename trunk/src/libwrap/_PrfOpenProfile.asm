        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   PrfOpenProfile:PROC
        PUBLIC  _PrfOpenProfile
_PrfOpenProfile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   PrfOpenProfile
    add    esp, 8
    pop    fs
    ret
_PrfOpenProfile ENDP

CODE32          ENDS

                END
