        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueueFromID:PROC
        PUBLIC  _WinQueueFromID
_WinQueueFromID PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinQueueFromID
    add    esp, 12
    pop    fs
    ret
_WinQueueFromID ENDP

CODE32          ENDS

                END
