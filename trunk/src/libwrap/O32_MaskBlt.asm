        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_MaskBlt:PROC
        PUBLIC  O32_MaskBlt
O32_MaskBlt PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    call   _O32_MaskBlt
    add    esp, 48
    pop    fs
    ret
O32_MaskBlt ENDP

CODE32          ENDS

                END
