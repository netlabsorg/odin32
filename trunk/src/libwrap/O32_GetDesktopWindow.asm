        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetDesktopWindow:PROC
        PUBLIC  O32_GetDesktopWindow
O32_GetDesktopWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetDesktopWindow
    pop    fs
    ret
O32_GetDesktopWindow ENDP

CODE32          ENDS

                END
