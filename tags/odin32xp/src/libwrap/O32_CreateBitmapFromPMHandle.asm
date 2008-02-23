        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreateBitmapFromPMHandle:PROC
        PUBLIC  O32_CreateBitmapFromPMHandle
O32_CreateBitmapFromPMHandle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CreateBitmapFromPMHandle
    add    esp, 4
    pop    fs
    ret
O32_CreateBitmapFromPMHandle ENDP

CODE32          ENDS

                END
