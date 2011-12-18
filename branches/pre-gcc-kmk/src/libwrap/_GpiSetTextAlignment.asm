        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetTextAlignment:PROC
        PUBLIC  _GpiSetTextAlignment
_GpiSetTextAlignment PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiSetTextAlignment
    add    esp, 12
    pop    fs
    ret
_GpiSetTextAlignment ENDP

CODE32          ENDS

                END
