        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetPolyFillMode:PROC
        PUBLIC  O32_GetPolyFillMode
O32_GetPolyFillMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetPolyFillMode
    add    esp, 4
    pop    fs
    ret
O32_GetPolyFillMode ENDP

CODE32          ENDS

                END
