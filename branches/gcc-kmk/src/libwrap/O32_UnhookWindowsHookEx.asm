        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_UnhookWindowsHookEx:PROC
        PUBLIC  O32_UnhookWindowsHookEx
O32_UnhookWindowsHookEx PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_UnhookWindowsHookEx
    add    esp, 4
    pop    fs
    ret
O32_UnhookWindowsHookEx ENDP

CODE32          ENDS

                END
