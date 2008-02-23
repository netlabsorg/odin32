        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_AngleArc:PROC
        PUBLIC  O32_AngleArc
O32_AngleArc PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   _O32_AngleArc
    add    esp, 24
    pop    fs
    ret
O32_AngleArc ENDP

CODE32          ENDS

                END
