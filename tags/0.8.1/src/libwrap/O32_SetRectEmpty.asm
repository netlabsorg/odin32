        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetRectEmpty:PROC
        PUBLIC  O32_SetRectEmpty
O32_SetRectEmpty PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SetRectEmpty
    add    esp, 4
    pop    fs
    ret
O32_SetRectEmpty ENDP

CODE32          ENDS

                END
