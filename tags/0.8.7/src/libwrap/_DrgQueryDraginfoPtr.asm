        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgQueryDraginfoPtr:PROC
        PUBLIC  _DrgQueryDraginfoPtr
_DrgQueryDraginfoPtr PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgQueryDraginfoPtr
    add    esp, 4
    pop    fs
    ret
_DrgQueryDraginfoPtr ENDP

CODE32          ENDS

                END
