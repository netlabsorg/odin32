        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryBitmapDimension:PROC
        PUBLIC  _GpiQueryBitmapDimension
_GpiQueryBitmapDimension PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryBitmapDimension
    add    esp, 8
    pop    fs
    ret
_GpiQueryBitmapDimension ENDP

CODE32          ENDS

                END
