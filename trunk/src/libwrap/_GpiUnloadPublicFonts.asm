        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiUnloadPublicFonts:PROC
        PUBLIC  _GpiUnloadPublicFonts
_GpiUnloadPublicFonts PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiUnloadPublicFonts
    add    esp, 8
    pop    fs
    ret
_GpiUnloadPublicFonts ENDP

CODE32          ENDS

                END
