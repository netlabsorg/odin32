        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinThreadAssocQueue:PROC
        PUBLIC  _WinThreadAssocQueue
_WinThreadAssocQueue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinThreadAssocQueue
    add    esp, 8
    pop    fs
    ret
_WinThreadAssocQueue ENDP

CODE32          ENDS

                END
