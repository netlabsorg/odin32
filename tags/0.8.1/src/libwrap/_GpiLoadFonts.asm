        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiLoadFonts:PROC
        PUBLIC  _GpiLoadFonts
_GpiLoadFonts PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiLoadFonts
    add    esp, 8
    pop    fs
    ret
_GpiLoadFonts ENDP

CODE32          ENDS

                END
