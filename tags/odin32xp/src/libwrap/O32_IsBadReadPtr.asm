        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_IsBadReadPtr:PROC
        PUBLIC  O32_IsBadReadPtr
O32_IsBadReadPtr PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   _O32_IsBadReadPtr
    add    esp, 8
    pop    fs
    ret
O32_IsBadReadPtr ENDP

CODE32          ENDS

                END
