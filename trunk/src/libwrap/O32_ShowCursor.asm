        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_ShowCursor:PROC
        PUBLIC  O32_ShowCursor
O32_ShowCursor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_ShowCursor
    add    esp, 4
    pop    fs
    ret
O32_ShowCursor ENDP

CODE32          ENDS

                END
