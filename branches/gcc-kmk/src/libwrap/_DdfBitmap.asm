        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DdfBitmap:PROC
        PUBLIC  _DdfBitmap
_DdfBitmap PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DdfBitmap
    add    esp, 12
    pop    fs
    ret
_DdfBitmap ENDP

CODE32          ENDS

                END
