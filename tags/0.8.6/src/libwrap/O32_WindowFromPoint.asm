        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_WindowFromPoint:PROC
        PUBLIC  O32_WindowFromPoint
O32_WindowFromPoint PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_WindowFromPoint
    add    esp, 4
    pop    fs
    ret
O32_WindowFromPoint ENDP

CODE32          ENDS

                END
