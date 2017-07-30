        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxClearMacroSpace:PROC
        PUBLIC  _RexxClearMacroSpace
_RexxClearMacroSpace PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   RexxClearMacroSpace
    pop    fs
    ret
_RexxClearMacroSpace ENDP

CODE32          ENDS

                END
