        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetQueueStatus:PROC
        PUBLIC  O32_GetQueueStatus
O32_GetQueueStatus PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetQueueStatus
    add    esp, 4
    pop    fs
    ret
O32_GetQueueStatus ENDP

CODE32          ENDS

                END
