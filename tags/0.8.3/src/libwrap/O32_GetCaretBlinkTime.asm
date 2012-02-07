        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetCaretBlinkTime:PROC
        PUBLIC  O32_GetCaretBlinkTime
O32_GetCaretBlinkTime PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetCaretBlinkTime
    pop    fs
    ret
O32_GetCaretBlinkTime ENDP

CODE32          ENDS

                END
