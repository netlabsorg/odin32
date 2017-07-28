        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiWCBitBlt:PROC
        PUBLIC  _GpiWCBitBlt
_GpiWCBitBlt PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   GpiWCBitBlt
    add    esp, 24
    pop    fs
    ret
_GpiWCBitBlt ENDP

CODE32          ENDS

                END
