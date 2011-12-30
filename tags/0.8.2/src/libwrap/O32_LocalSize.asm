        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_LocalSize:PROC
        PUBLIC  O32_LocalSize
O32_LocalSize PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_LocalSize
    add    esp, 4
    pop    fs
    ret
O32_LocalSize ENDP

CODE32          ENDS

                END
