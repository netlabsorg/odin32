        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgAllocDragtransfer:PROC
        PUBLIC  _DrgAllocDragtransfer
_DrgAllocDragtransfer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgAllocDragtransfer
    add    esp, 4
    pop    fs
    ret
_DrgAllocDragtransfer ENDP

CODE32          ENDS

                END
