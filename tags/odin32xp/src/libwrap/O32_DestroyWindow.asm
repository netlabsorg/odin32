        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DestroyWindow:PROC
        PUBLIC  O32_DestroyWindow
O32_DestroyWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_DestroyWindow
    add    esp, 4
    pop    fs
    ret
O32_DestroyWindow ENDP

CODE32          ENDS

                END
