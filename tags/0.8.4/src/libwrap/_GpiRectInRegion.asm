        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiRectInRegion:PROC
        PUBLIC  _GpiRectInRegion
_GpiRectInRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiRectInRegion
    add    esp, 12
    pop    fs
    ret
_GpiRectInRegion ENDP

CODE32          ENDS

                END
