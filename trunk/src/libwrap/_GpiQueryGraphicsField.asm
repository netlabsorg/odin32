        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryGraphicsField:PROC
        PUBLIC  _GpiQueryGraphicsField
_GpiQueryGraphicsField PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryGraphicsField
    add    esp, 8
    pop    fs
    ret
_GpiQueryGraphicsField ENDP

CODE32          ENDS

                END
