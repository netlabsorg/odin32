        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_EndPage:PROC
        PUBLIC  O32_EndPage
O32_EndPage PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_EndPage
    add    esp, 4
    pop    fs
    ret
O32_EndPage ENDP

CODE32          ENDS

                END
