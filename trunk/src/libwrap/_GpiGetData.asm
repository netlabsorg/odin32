        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiGetData:PROC
        PUBLIC  _GpiGetData
_GpiGetData PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   GpiGetData
    add    esp, 24
    pop    fs
    ret
_GpiGetData ENDP

CODE32          ENDS

                END
