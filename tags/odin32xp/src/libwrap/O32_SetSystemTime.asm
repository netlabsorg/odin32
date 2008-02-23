        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetSystemTime:PROC
        PUBLIC  O32_SetSystemTime
O32_SetSystemTime PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SetSystemTime
    add    esp, 4
    pop    fs
    ret
O32_SetSystemTime ENDP

CODE32          ENDS

                END
