        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   PrfCloseProfile:PROC
        PUBLIC  _PrfCloseProfile
_PrfCloseProfile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   PrfCloseProfile
    add    esp, 4
    pop    fs
    ret
_PrfCloseProfile ENDP

CODE32          ENDS

                END
