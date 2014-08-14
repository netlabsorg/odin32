        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetDoubleClickTime:PROC
        PUBLIC  O32_GetDoubleClickTime
O32_GetDoubleClickTime PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetDoubleClickTime
    pop    fs
    ret
O32_GetDoubleClickTime ENDP

CODE32          ENDS

                END
