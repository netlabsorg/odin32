        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreateDIBitmap:PROC
        PUBLIC  O32_CreateDIBitmap
O32_CreateDIBitmap PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   _O32_CreateDIBitmap
    add    esp, 24
    pop    fs
    ret
O32_CreateDIBitmap ENDP

CODE32          ENDS

                END
