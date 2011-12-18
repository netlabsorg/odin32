        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetGraphicsField:PROC
        PUBLIC  _GpiSetGraphicsField
_GpiSetGraphicsField PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetGraphicsField
    add    esp, 8
    pop    fs
    ret
_GpiSetGraphicsField ENDP

CODE32          ENDS

                END
