        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetFileTitle:PROC
        PUBLIC  O32_GetFileTitle
O32_GetFileTitle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   _O32_GetFileTitle
    add    esp, 12
    pop    fs
    ret
O32_GetFileTitle ENDP

CODE32          ENDS

                END
