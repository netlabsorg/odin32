        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQuerySetIds:PROC
        PUBLIC  _GpiQuerySetIds
_GpiQuerySetIds PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiQuerySetIds
    add    esp, 20
    pop    fs
    ret
_GpiQuerySetIds ENDP

CODE32          ENDS

                END
