        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetGraphicsMode:PROC
        PUBLIC  O32_GetGraphicsMode
O32_GetGraphicsMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetGraphicsMode
    add    esp, 4
    pop    fs
    ret
O32_GetGraphicsMode ENDP

CODE32          ENDS

                END
