        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_MessageBeep:PROC
        PUBLIC  O32_MessageBeep
O32_MessageBeep PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_MessageBeep
    add    esp, 4
    pop    fs
    ret
O32_MessageBeep ENDP

CODE32          ENDS

                END
