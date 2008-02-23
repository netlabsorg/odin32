        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiExcludeClipRectangle:PROC
        PUBLIC  _GpiExcludeClipRectangle
_GpiExcludeClipRectangle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiExcludeClipRectangle
    add    esp, 8
    pop    fs
    ret
_GpiExcludeClipRectangle ENDP

CODE32          ENDS

                END
