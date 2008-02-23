        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DispatchMessage:PROC
        PUBLIC  O32_DispatchMessage
O32_DispatchMessage PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_DispatchMessage
    add    esp, 4
    pop    fs
    ret
O32_DispatchMessage ENDP

CODE32          ENDS

                END
