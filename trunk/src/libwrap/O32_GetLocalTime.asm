        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetLocalTime:PROC
        PUBLIC  O32_GetLocalTime
O32_GetLocalTime PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetLocalTime
    add    esp, 4
    pop    fs
    ret
O32_GetLocalTime ENDP

CODE32          ENDS

                END
