        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiDeleteSegment:PROC
        PUBLIC  _GpiDeleteSegment
_GpiDeleteSegment PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiDeleteSegment
    add    esp, 8
    pop    fs
    ret
_GpiDeleteSegment ENDP

CODE32          ENDS

                END
