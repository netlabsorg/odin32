        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetClipBox:PROC
        PUBLIC  O32_GetClipBox
O32_GetClipBox PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   _O32_GetClipBox
    add    esp, 8
    pop    fs
    ret
O32_GetClipBox ENDP

CODE32          ENDS

                END
