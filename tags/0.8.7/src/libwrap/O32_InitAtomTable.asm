        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_InitAtomTable:PROC
        PUBLIC  O32_InitAtomTable
O32_InitAtomTable PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_InitAtomTable
    add    esp, 4
    pop    fs
    ret
O32_InitAtomTable ENDP

CODE32          ENDS

                END
