        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryLineWidth:PROC
        PUBLIC  _GpiQueryLineWidth
_GpiQueryLineWidth PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryLineWidth
    add    esp, 4
    pop    fs
    ret
_GpiQueryLineWidth ENDP

CODE32          ENDS

                END
