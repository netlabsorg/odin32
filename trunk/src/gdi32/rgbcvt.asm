; $Id: rgbcvt.asm,v 1.2 2000-02-03 18:06:36 achimha Exp $
; Copyright 2000 Daniela Engert (dani@ngrt.de)

	.386p
	.MODEL FLAT
	.CODE

; void _Optlink RGB555to565 (WORD *dest, WORD *src, ULONG num);

	PUBLIC RGB555to565

RGB555to565 PROC NEAR

	PUSH	ESI
	PUSH	EDI
	CLD

	MOV	EDI, EAX		; _Optlink arg1 = EAX
	MOV	ESI, EDX		; _Optlink arg2 = EDX
	SHR	ECX, 1			; _Optlink arg3 = ECX
	PUSHF
cvt:
	LODSD
	MOV	EDX, EAX
	AND	EDX, 001F001Fh
	SHL	EAX, 1
	AND	EAX, 0FFC0FFC0h
	OR	EAX, EDX
	STOSD
	LOOP	cvt

	POPF
	JNC	SHORT done

	LODSW
	MOV	DX, AX
	AND	DX, 001Fh
	SHL	AX, 1
	AND	AX, 0FFC0h
	OR	AX, DX
	STOSW

done:
	POP	EDI
	POP	ESI
	RET

	ENDP

	END
