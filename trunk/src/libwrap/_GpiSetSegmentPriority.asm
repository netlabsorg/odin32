        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetSegmentPriority:PROC
        PUBLIC  _GpiSetSegmentPriority
_GpiSetSegmentPriority PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiSetSegmentPriority
    add    esp, 16
    pop    fs
    ret
_GpiSetSegmentPriority ENDP

CODE32          ENDS

                END
