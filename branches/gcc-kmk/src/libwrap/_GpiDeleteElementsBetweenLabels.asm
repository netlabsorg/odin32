        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiDeleteElementsBetweenLabels:PROC
        PUBLIC  _GpiDeleteElementsBetweenLabels
_GpiDeleteElementsBetweenLabels PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiDeleteElementsBetweenLabels
    add    esp, 12
    pop    fs
    ret
_GpiDeleteElementsBetweenLabels ENDP

CODE32          ENDS

                END
