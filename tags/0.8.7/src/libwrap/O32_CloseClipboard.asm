        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CloseClipboard:PROC
        PUBLIC  O32_CloseClipboard
O32_CloseClipboard PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_CloseClipboard
    pop    fs
    ret
O32_CloseClipboard ENDP

CODE32          ENDS

                END
