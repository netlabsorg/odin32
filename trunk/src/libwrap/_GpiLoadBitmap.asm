        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiLoadBitmap:PROC
        PUBLIC  _GpiLoadBitmap
_GpiLoadBitmap PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiLoadBitmap
    add    esp, 20
    pop    fs
    ret
_GpiLoadBitmap ENDP

CODE32          ENDS

                END
