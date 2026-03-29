		include "macros.s"
_start:		LDI   $00, _stack
		BL    $00, $00, _MAIN
		HLT
_FOO:		ENTER $04
		LDW   $08, $07, $0000
		LDW   $09, $07, $0004
		ADD   $08, $08, $09
		ORR   $02, $08, $08
		LEAVE
		LEAVE
_MAIN:		ENTER $04
		LDI   $08, $00000000
		PUS   $08
		LDI   $08, $00000008
		LDI   $09, $00000008
		ORR   $04, $09, $09
		ORR   $03, $08, $08
		BL    $00, $00, _FOO
		ORR   $0a, $02, $02
		STW   $0a, $07, $0010
		LDW   $0a, $07, $0010
		ORR   $02, $0a, $0a
		LEAVE
		LEAVE
		times 4096 - ($ - $$) db 0
_stack:
