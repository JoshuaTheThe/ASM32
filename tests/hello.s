		include "macros.s"
_start:		LDI   $00, _stack
		BL    $00, $00, _MAIN
		HLT
_FOO:		ENTER $04
		LEAVE
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
_MAIN:		ENTER $04
		LDI   $08, $00000042
		ORR   $03, $08, $08
		BL    $00, $00, _PUTCHAR
		ORR   $09, $02, $02
		LEAVE
		times 4096 - ($ - $$) db 0
_stack:
