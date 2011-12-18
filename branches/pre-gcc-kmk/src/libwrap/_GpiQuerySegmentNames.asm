        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQuerySegmentNames:PROC
        PUBLIC  _GpiQuerySegmentNames
_GpiQuerySegmentNames PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiQuerySegmentNames
    add    esp, 20
    pop    fs
    ret
_GpiQuerySegmentNames ENDP

CODE32          ENDS

                END
