        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiUnloadFonts:PROC
        PUBLIC  _GpiUnloadFonts
_GpiUnloadFonts PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiUnloadFonts
    add    esp, 8
    pop    fs
    ret
_GpiUnloadFonts ENDP

CODE32          ENDS

                END
