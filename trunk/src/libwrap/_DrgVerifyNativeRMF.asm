        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgVerifyNativeRMF:PROC
        PUBLIC  _DrgVerifyNativeRMF
_DrgVerifyNativeRMF PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DrgVerifyNativeRMF
    add    esp, 8
    pop    fs
    ret
_DrgVerifyNativeRMF ENDP

CODE32          ENDS

                END
