        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_RegisterClipboardFormat:PROC
        PUBLIC  O32_RegisterClipboardFormat
O32_RegisterClipboardFormat PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_RegisterClipboardFormat
    add    esp, 4
    pop    fs
    ret
O32_RegisterClipboardFormat ENDP

CODE32          ENDS

                END
