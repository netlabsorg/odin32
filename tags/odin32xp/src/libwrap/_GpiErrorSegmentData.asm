        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiErrorSegmentData:PROC
        PUBLIC  _GpiErrorSegmentData
_GpiErrorSegmentData PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiErrorSegmentData
    add    esp, 12
    pop    fs
    ret
_GpiErrorSegmentData ENDP

CODE32          ENDS

                END
