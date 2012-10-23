        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_EnumClipboardFormats:PROC
        PUBLIC  O32_EnumClipboardFormats
O32_EnumClipboardFormats PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_EnumClipboardFormats
    add    esp, 4
    pop    fs
    ret
O32_EnumClipboardFormats ENDP

CODE32          ENDS

                END
