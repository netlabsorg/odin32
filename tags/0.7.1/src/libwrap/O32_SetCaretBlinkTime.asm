        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetCaretBlinkTime:PROC
        PUBLIC  O32_SetCaretBlinkTime
O32_SetCaretBlinkTime PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SetCaretBlinkTime
    add    esp, 4
    pop    fs
    ret
O32_SetCaretBlinkTime ENDP

CODE32          ENDS

                END
