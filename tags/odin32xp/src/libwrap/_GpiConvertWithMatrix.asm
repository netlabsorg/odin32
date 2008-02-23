        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiConvertWithMatrix:PROC
        PUBLIC  _GpiConvertWithMatrix
_GpiConvertWithMatrix PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiConvertWithMatrix
    add    esp, 20
    pop    fs
    ret
_GpiConvertWithMatrix ENDP

CODE32          ENDS

                END
