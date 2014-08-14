        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosFSCtl:PROC
        PUBLIC  _DosFSCtl
_DosFSCtl PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    call   DosFSCtl
    add    esp, 40
    pop    fs
    ret
_DosFSCtl ENDP

CODE32          ENDS

                END
