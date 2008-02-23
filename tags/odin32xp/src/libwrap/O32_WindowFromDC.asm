        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_WindowFromDC:PROC
        PUBLIC  O32_WindowFromDC
O32_WindowFromDC PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_WindowFromDC
    add    esp, 4
    pop    fs
    ret
O32_WindowFromDC ENDP

CODE32          ENDS

                END
