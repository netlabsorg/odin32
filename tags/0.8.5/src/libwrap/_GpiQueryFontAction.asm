        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryFontAction:PROC
        PUBLIC  _GpiQueryFontAction
_GpiQueryFontAction PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryFontAction
    add    esp, 8
    pop    fs
    ret
_GpiQueryFontAction ENDP

CODE32          ENDS

                END
