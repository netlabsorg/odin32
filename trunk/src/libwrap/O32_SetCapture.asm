        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetCapture:PROC
        PUBLIC  O32_SetCapture
O32_SetCapture PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SetCapture
    add    esp, 4
    pop    fs
    ret
O32_SetCapture ENDP

CODE32          ENDS

                END
