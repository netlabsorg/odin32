        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiFloodFill:PROC
        PUBLIC  _GpiFloodFill
_GpiFloodFill PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiFloodFill
    add    esp, 12
    pop    fs
    ret
_GpiFloodFill ENDP

CODE32          ENDS

                END
