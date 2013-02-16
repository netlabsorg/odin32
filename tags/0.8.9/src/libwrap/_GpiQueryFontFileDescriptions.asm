        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryFontFileDescriptions:PROC
        PUBLIC  _GpiQueryFontFileDescriptions
_GpiQueryFontFileDescriptions PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiQueryFontFileDescriptions
    add    esp, 16
    pop    fs
    ret
_GpiQueryFontFileDescriptions ENDP

CODE32          ENDS

                END
