        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_IsZoomed:PROC
        PUBLIC  O32_IsZoomed
O32_IsZoomed PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_IsZoomed
    add    esp, 4
    pop    fs
    ret
O32_IsZoomed ENDP

CODE32          ENDS

                END
