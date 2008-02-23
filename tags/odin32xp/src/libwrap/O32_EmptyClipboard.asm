        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_EmptyClipboard:PROC
        PUBLIC  O32_EmptyClipboard
O32_EmptyClipboard PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_EmptyClipboard
    pop    fs
    ret
O32_EmptyClipboard ENDP

CODE32          ENDS

                END
