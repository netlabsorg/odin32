        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryLogColorTable:PROC
        PUBLIC  _GpiQueryLogColorTable
_GpiQueryLogColorTable PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiQueryLogColorTable
    add    esp, 20
    pop    fs
    ret
_GpiQueryLogColorTable ENDP

CODE32          ENDS

                END
