        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetClipRgn:PROC
        PUBLIC  O32_GetClipRgn
O32_GetClipRgn PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   _O32_GetClipRgn
    add    esp, 8
    pop    fs
    ret
O32_GetClipRgn ENDP

CODE32          ENDS

                END
