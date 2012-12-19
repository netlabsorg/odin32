        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_WinQueryTranslateMode:PROC
        PUBLIC  O32_WinQueryTranslateMode
O32_WinQueryTranslateMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_WinQueryTranslateMode
    pop    fs
    ret
O32_WinQueryTranslateMode ENDP

CODE32          ENDS

                END
