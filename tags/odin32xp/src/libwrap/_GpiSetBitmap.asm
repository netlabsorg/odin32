        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetBitmap:PROC
        PUBLIC  _GpiSetBitmap
_GpiSetBitmap PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetBitmap
    add    esp, 8
    pop    fs
    ret
_GpiSetBitmap ENDP

CODE32          ENDS

                END
