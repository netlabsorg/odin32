        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetProfileString:PROC
        PUBLIC  O32_GetProfileString
O32_GetProfileString PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   _O32_GetProfileString
    add    esp, 20
    pop    fs
    ret
O32_GetProfileString ENDP

CODE32          ENDS

                END
