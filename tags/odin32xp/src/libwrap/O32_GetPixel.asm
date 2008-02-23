        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetPixel:PROC
        PUBLIC  O32_GetPixel
O32_GetPixel PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   _O32_GetPixel
    add    esp, 12
    pop    fs
    ret
O32_GetPixel ENDP

CODE32          ENDS

                END
