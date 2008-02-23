        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgVerifyRMF:PROC
        PUBLIC  _DrgVerifyRMF
_DrgVerifyRMF PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DrgVerifyRMF
    add    esp, 12
    pop    fs
    ret
_DrgVerifyRMF ENDP

CODE32          ENDS

                END
