        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_IsRectEmpty:PROC
        PUBLIC  O32_IsRectEmpty
O32_IsRectEmpty PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_IsRectEmpty
    add    esp, 4
    pop    fs
    ret
O32_IsRectEmpty ENDP

CODE32          ENDS

                END
