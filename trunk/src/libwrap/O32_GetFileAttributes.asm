        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetFileAttributes:PROC
        PUBLIC  O32_GetFileAttributes
O32_GetFileAttributes PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetFileAttributes
    add    esp, 4
    pop    fs
    ret
O32_GetFileAttributes ENDP

CODE32          ENDS

                END
