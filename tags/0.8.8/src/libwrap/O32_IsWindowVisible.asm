        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_IsWindowVisible:PROC
        PUBLIC  O32_IsWindowVisible
O32_IsWindowVisible PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_IsWindowVisible
    add    esp, 4
    pop    fs
    ret
O32_IsWindowVisible ENDP

CODE32          ENDS

                END
