        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryRegionRects:PROC
        PUBLIC  _GpiQueryRegionRects
_GpiQueryRegionRects PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiQueryRegionRects
    add    esp, 20
    pop    fs
    ret
_GpiQueryRegionRects ENDP

CODE32          ENDS

                END
