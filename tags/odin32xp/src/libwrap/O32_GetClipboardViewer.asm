        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetClipboardViewer:PROC
        PUBLIC  O32_GetClipboardViewer
O32_GetClipboardViewer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetClipboardViewer
    pop    fs
    ret
O32_GetClipboardViewer ENDP

CODE32          ENDS

                END
