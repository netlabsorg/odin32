        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCreateRoundRectRegion:PROC
        PUBLIC  _GpiCreateRoundRectRegion
_GpiCreateRoundRectRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiCreateRoundRectRegion
    add    esp, 16
    pop    fs
    ret
_GpiCreateRoundRectRegion ENDP

CODE32          ENDS

                END
