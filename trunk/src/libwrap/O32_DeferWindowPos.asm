        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DeferWindowPos:PROC
        PUBLIC  O32_DeferWindowPos
O32_DeferWindowPos PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    call   _O32_DeferWindowPos
    add    esp, 32
    pop    fs
    ret
O32_DeferWindowPos ENDP

CODE32          ENDS

                END
