        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreatePopupMenu:PROC
        PUBLIC  O32_CreatePopupMenu
O32_CreatePopupMenu PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_CreatePopupMenu
    pop    fs
    ret
O32_CreatePopupMenu ENDP

CODE32          ENDS

                END
