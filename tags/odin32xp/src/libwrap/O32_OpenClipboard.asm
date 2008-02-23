        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_OpenClipboard:PROC
        PUBLIC  O32_OpenClipboard
O32_OpenClipboard PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_OpenClipboard
    add    esp, 4
    pop    fs
    ret
O32_OpenClipboard ENDP

CODE32          ENDS

                END
