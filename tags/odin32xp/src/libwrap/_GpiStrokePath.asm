        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiStrokePath:PROC
        PUBLIC  _GpiStrokePath
_GpiStrokePath PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiStrokePath
    add    esp, 12
    pop    fs
    ret
_GpiStrokePath ENDP

CODE32          ENDS

                END
