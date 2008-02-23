        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetSysColor:PROC
        PUBLIC  O32_GetSysColor
O32_GetSysColor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetSysColor
    add    esp, 4
    pop    fs
    ret
O32_GetSysColor ENDP

CODE32          ENDS

                END
