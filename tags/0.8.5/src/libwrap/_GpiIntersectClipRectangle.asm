        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiIntersectClipRectangle:PROC
        PUBLIC  _GpiIntersectClipRectangle
_GpiIntersectClipRectangle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiIntersectClipRectangle
    add    esp, 8
    pop    fs
    ret
_GpiIntersectClipRectangle ENDP

CODE32          ENDS

                END
