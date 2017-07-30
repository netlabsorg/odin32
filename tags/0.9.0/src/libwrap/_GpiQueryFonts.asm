        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryFonts:PROC
        PUBLIC  _GpiQueryFonts
_GpiQueryFonts PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   GpiQueryFonts
    add    esp, 24
    pop    fs
    ret
_GpiQueryFonts ENDP

CODE32          ENDS

                END
