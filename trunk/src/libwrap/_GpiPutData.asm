        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiPutData:PROC
        PUBLIC  _GpiPutData
_GpiPutData PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiPutData
    add    esp, 16
    pop    fs
    ret
_GpiPutData ENDP

CODE32          ENDS

                END
