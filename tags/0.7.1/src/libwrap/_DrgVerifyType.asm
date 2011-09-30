        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgVerifyType:PROC
        PUBLIC  _DrgVerifyType
_DrgVerifyType PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DrgVerifyType
    add    esp, 8
    pop    fs
    ret
_DrgVerifyType ENDP

CODE32          ENDS

                END
