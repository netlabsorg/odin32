        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetLineWidth:PROC
        PUBLIC  _GpiSetLineWidth
_GpiSetLineWidth PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetLineWidth
    add    esp, 8
    pop    fs
    ret
_GpiSetLineWidth ENDP

CODE32          ENDS

                END
