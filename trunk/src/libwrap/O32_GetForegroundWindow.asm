        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetForegroundWindow:PROC
        PUBLIC  O32_GetForegroundWindow
O32_GetForegroundWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetForegroundWindow
    pop    fs
    ret
O32_GetForegroundWindow ENDP

CODE32          ENDS

                END
