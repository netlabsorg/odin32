        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetLineEnd:PROC
        PUBLIC  _GpiSetLineEnd
_GpiSetLineEnd PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetLineEnd
    add    esp, 8
    pop    fs
    ret
_GpiSetLineEnd ENDP

CODE32          ENDS

                END
