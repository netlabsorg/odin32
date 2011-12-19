        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetEvent:PROC
        PUBLIC  O32_SetEvent
O32_SetEvent PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SetEvent
    add    esp, 4
    pop    fs
    ret
O32_SetEvent ENDP

CODE32          ENDS

                END
