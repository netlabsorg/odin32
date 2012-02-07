        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreateBitmapIndirect:PROC
        PUBLIC  O32_CreateBitmapIndirect
O32_CreateBitmapIndirect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CreateBitmapIndirect
    add    esp, 4
    pop    fs
    ret
O32_CreateBitmapIndirect ENDP

CODE32          ENDS

                END
