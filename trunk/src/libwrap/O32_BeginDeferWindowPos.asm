        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_BeginDeferWindowPos:PROC
        PUBLIC  O32_BeginDeferWindowPos
O32_BeginDeferWindowPos PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_BeginDeferWindowPos
    add    esp, 4
    pop    fs
    ret
O32_BeginDeferWindowPos ENDP

CODE32          ENDS

                END
