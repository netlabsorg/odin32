        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosFreeThreadLocalMemory:PROC
        PUBLIC  _DosFreeThreadLocalMemory
_DosFreeThreadLocalMemory PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosFreeThreadLocalMemory
    add    esp, 4
    pop    fs
    ret
_DosFreeThreadLocalMemory ENDP

CODE32          ENDS

                END
