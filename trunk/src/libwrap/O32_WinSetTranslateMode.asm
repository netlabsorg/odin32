        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_WinSetTranslateMode:PROC
        PUBLIC  O32_WinSetTranslateMode
O32_WinSetTranslateMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_WinSetTranslateMode
    add    esp, 4
    pop    fs
    ret
O32_WinSetTranslateMode ENDP

CODE32          ENDS

                END
