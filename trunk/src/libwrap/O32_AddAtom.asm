        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_AddAtom:PROC
        PUBLIC  O32_AddAtom
O32_AddAtom PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_AddAtom
    add    esp, 4
    pop    fs
    ret
O32_AddAtom ENDP

CODE32          ENDS

                END
