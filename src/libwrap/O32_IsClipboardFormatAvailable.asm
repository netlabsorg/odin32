        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_IsClipboardFormatAvailable:PROC
        PUBLIC  O32_IsClipboardFormatAvailable
O32_IsClipboardFormatAvailable PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_IsClipboardFormatAvailable
    add    esp, 4
    pop    fs
    ret
O32_IsClipboardFormatAvailable ENDP

CODE32          ENDS

                END
