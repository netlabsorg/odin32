        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_PostQuitMessage:PROC
        PUBLIC  O32_PostQuitMessage
O32_PostQuitMessage PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_PostQuitMessage
    add    esp, 4
    pop    fs
    ret
O32_PostQuitMessage ENDP

CODE32          ENDS

                END
