        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetPageViewport:PROC
        PUBLIC  _GpiSetPageViewport
_GpiSetPageViewport PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetPageViewport
    add    esp, 8
    pop    fs
    ret
_GpiSetPageViewport ENDP

CODE32          ENDS

                END
