        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetActiveWindow:PROC
        PUBLIC  O32_GetActiveWindow
O32_GetActiveWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetActiveWindow
    pop    fs
    ret
O32_GetActiveWindow ENDP

CODE32          ENDS

                END
