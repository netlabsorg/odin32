        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetStockObject:PROC
        PUBLIC  O32_GetStockObject
O32_GetStockObject PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetStockObject
    add    esp, 4
    pop    fs
    ret
O32_GetStockObject ENDP

CODE32          ENDS

                END
