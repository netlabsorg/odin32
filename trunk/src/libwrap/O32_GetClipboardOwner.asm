        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetClipboardOwner:PROC
        PUBLIC  O32_GetClipboardOwner
O32_GetClipboardOwner PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetClipboardOwner
    pop    fs
    ret
O32_GetClipboardOwner ENDP

CODE32          ENDS

                END
