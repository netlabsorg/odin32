        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_StretchBlt:PROC
        PUBLIC  O32_StretchBlt
O32_StretchBlt PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+48]
    push   dword ptr [esp+48]
    push   dword ptr [esp+48]
    push   dword ptr [esp+48]
    push   dword ptr [esp+48]
    push   dword ptr [esp+48]
    push   dword ptr [esp+48]
    push   dword ptr [esp+48]
    push   dword ptr [esp+48]
    push   dword ptr [esp+48]
    push   dword ptr [esp+48]
    call   _O32_StretchBlt
    add    esp, 44
    pop    fs
    ret
O32_StretchBlt ENDP

CODE32          ENDS

                END
