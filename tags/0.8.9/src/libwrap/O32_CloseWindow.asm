        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CloseWindow:PROC
        PUBLIC  O32_CloseWindow
O32_CloseWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CloseWindow
    add    esp, 4
    pop    fs
    ret
O32_CloseWindow ENDP

CODE32          ENDS

                END
