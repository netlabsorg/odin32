        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetDoubleClickTime:PROC
        PUBLIC  O32_SetDoubleClickTime
O32_SetDoubleClickTime PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SetDoubleClickTime
    add    esp, 4
    pop    fs
    ret
O32_SetDoubleClickTime ENDP

CODE32          ENDS

                END
