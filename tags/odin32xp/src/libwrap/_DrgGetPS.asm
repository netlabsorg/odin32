        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgGetPS:PROC
        PUBLIC  _DrgGetPS
_DrgGetPS PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgGetPS
    add    esp, 4
    pop    fs
    ret
_DrgGetPS ENDP

CODE32          ENDS

                END
