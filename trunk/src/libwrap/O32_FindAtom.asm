        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_FindAtom:PROC
        PUBLIC  O32_FindAtom
O32_FindAtom PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_FindAtom
    add    esp, 4
    pop    fs
    ret
O32_FindAtom ENDP

CODE32          ENDS

                END
