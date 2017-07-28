        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiPtInRegion:PROC
        PUBLIC  _GpiPtInRegion
_GpiPtInRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiPtInRegion
    add    esp, 12
    pop    fs
    ret
_GpiPtInRegion ENDP

CODE32          ENDS

                END
