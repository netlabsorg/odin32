        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetClipboardData:PROC
        PUBLIC  O32_GetClipboardData
O32_GetClipboardData PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetClipboardData
    add    esp, 4
    pop    fs
    ret
O32_GetClipboardData ENDP

CODE32          ENDS

                END
