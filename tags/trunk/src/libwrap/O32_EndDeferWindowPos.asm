        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_EndDeferWindowPos:PROC
        PUBLIC  O32_EndDeferWindowPos
O32_EndDeferWindowPos PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_EndDeferWindowPos
    add    esp, 4
    pop    fs
    ret
O32_EndDeferWindowPos ENDP

CODE32          ENDS

                END
