        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetClipboardViewer:PROC
        PUBLIC  O32_SetClipboardViewer
O32_SetClipboardViewer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SetClipboardViewer
    add    esp, 4
    pop    fs
    ret
O32_SetClipboardViewer ENDP

CODE32          ENDS

                END
