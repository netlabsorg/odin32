        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgQueryStrName:PROC
        PUBLIC  _DrgQueryStrName
_DrgQueryStrName PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DrgQueryStrName
    add    esp, 12
    pop    fs
    ret
_DrgQueryStrName ENDP

CODE32          ENDS

                END
