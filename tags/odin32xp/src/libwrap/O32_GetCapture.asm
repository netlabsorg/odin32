        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetCapture:PROC
        PUBLIC  O32_GetCapture
O32_GetCapture PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetCapture
    pop    fs
    ret
O32_GetCapture ENDP

CODE32          ENDS

                END
