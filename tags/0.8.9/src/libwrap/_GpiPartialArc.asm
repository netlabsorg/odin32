        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiPartialArc:PROC
        PUBLIC  _GpiPartialArc
_GpiPartialArc PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiPartialArc
    add    esp, 20
    pop    fs
    ret
_GpiPartialArc ENDP

CODE32          ENDS

                END
