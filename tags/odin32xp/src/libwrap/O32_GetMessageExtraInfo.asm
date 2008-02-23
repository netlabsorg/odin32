        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetMessageExtraInfo:PROC
        PUBLIC  O32_GetMessageExtraInfo
O32_GetMessageExtraInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetMessageExtraInfo
    pop    fs
    ret
O32_GetMessageExtraInfo ENDP

CODE32          ENDS

                END
