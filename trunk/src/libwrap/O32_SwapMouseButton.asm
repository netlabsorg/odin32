        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SwapMouseButton:PROC
        PUBLIC  O32_SwapMouseButton
O32_SwapMouseButton PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SwapMouseButton
    add    esp, 4
    pop    fs
    ret
O32_SwapMouseButton ENDP

CODE32          ENDS

                END
