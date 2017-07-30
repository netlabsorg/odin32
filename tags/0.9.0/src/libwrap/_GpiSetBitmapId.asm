        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetBitmapId:PROC
        PUBLIC  _GpiSetBitmapId
_GpiSetBitmapId PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiSetBitmapId
    add    esp, 12
    pop    fs
    ret
_GpiSetBitmapId ENDP

CODE32          ENDS

                END
