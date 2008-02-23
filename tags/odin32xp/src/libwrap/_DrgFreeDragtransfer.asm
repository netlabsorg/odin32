        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgFreeDragtransfer:PROC
        PUBLIC  _DrgFreeDragtransfer
_DrgFreeDragtransfer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgFreeDragtransfer
    add    esp, 4
    pop    fs
    ret
_DrgFreeDragtransfer ENDP

CODE32          ENDS

                END
