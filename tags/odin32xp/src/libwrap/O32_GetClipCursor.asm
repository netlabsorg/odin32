        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetClipCursor:PROC
        PUBLIC  O32_GetClipCursor
O32_GetClipCursor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetClipCursor
    add    esp, 4
    pop    fs
    ret
O32_GetClipCursor ENDP

CODE32          ENDS

                END
