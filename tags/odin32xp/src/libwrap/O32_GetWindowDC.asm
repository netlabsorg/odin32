        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetWindowDC:PROC
        PUBLIC  O32_GetWindowDC
O32_GetWindowDC PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetWindowDC
    add    esp, 4
    pop    fs
    ret
O32_GetWindowDC ENDP

CODE32          ENDS

                END
