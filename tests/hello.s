		include "macros.s"
_start:		LDI   $00, _stack
		BL    $00, $00, _MAIN
		HLT
_PUTCHAR:		ENTER $04
		LDI   $08, $00000000
		PUS   $08
		LDI   $08, $00002000
		LDI   $09, $00000004
		SUB   $08, $08, $09
		STW   $08, $07, $0010
		LDB   $08, $07, $0000
		LDW   $09, $07, $0010
		STB   $08, $09, $0000
		LEAVE
_LENGTH:		ENTER $04
		LDI   $08, $00000000
		PUS   $08
		LDW   $08, $07, $0000
		LDB   $08, $08, $00
		STB   $08, $07, $0010
		LDB   $08, $07, $0010
		ORR   $02, $08, $08
		LEAVE
		LEAVE
_PUTSTRING:		ENTER $04
		LDW   $08, $07, $0000
		ORR   $03, $08, $08
		BL    $00, $00, _LENGTH
		ORR   $09, $02, $02
		LEAVE
_MAIN:		ENTER $04
		LDI   $0a, $00000042
		ORR   $03, $0a, $0a
		BL    $00, $00, _PUTCHAR
		ORR   $0b, $02, $02
		LEAVE
		times 4096 - ($ - $$) db 0
_stack:
