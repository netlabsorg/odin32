        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetPattern:PROC
        PUBLIC  _GpiSetPattern
_GpiSetPattern PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetPattern
    add    esp, 8
    pop    fs
    ret
_GpiSetPattern ENDP

CODE32          ENDS

                END
