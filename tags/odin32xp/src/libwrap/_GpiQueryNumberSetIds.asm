        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryNumberSetIds:PROC
        PUBLIC  _GpiQueryNumberSetIds
_GpiQueryNumberSetIds PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryNumberSetIds
    add    esp, 4
    pop    fs
    ret
_GpiQueryNumberSetIds ENDP

CODE32          ENDS

                END
