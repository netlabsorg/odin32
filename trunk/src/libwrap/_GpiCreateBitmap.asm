        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCreateBitmap:PROC
        PUBLIC  _GpiCreateBitmap
_GpiCreateBitmap PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiCreateBitmap
    add    esp, 20
    pop    fs
    ret
_GpiCreateBitmap ENDP

CODE32          ENDS

                END
