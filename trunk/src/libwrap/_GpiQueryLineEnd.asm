        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryLineEnd:PROC
        PUBLIC  _GpiQueryLineEnd
_GpiQueryLineEnd PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryLineEnd
    add    esp, 4
    pop    fs
    ret
_GpiQueryLineEnd ENDP

CODE32          ENDS

                END
