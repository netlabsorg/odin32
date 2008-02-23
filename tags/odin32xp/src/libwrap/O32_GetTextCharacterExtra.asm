        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetTextCharacterExtra:PROC
        PUBLIC  O32_GetTextCharacterExtra
O32_GetTextCharacterExtra PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetTextCharacterExtra
    add    esp, 4
    pop    fs
    ret
O32_GetTextCharacterExtra ENDP

CODE32          ENDS

                END
