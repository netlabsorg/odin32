        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgQueryTrueTypeLen:PROC
        PUBLIC  _DrgQueryTrueTypeLen
_DrgQueryTrueTypeLen PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgQueryTrueTypeLen
    add    esp, 4
    pop    fs
    ret
_DrgQueryTrueTypeLen ENDP

CODE32          ENDS

                END
