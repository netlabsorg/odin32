        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_MoveWindow:PROC
        PUBLIC  O32_MoveWindow
O32_MoveWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   _O32_MoveWindow
    add    esp, 24
    pop    fs
    ret
O32_MoveWindow ENDP

CODE32          ENDS

                END
