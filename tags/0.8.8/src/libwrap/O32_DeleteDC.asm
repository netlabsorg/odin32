        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DeleteDC:PROC
        PUBLIC  O32_DeleteDC
O32_DeleteDC PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_DeleteDC
    add    esp, 4
    pop    fs
    ret
O32_DeleteDC ENDP

CODE32          ENDS

                END
