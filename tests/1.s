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
		LDW   $08, $07, $0004
		LDB   $08, $08, $00
		LDI   $08, $00000045
		LDI   $09, $00000044
		ORR   $04, $09, $09
		ORR   $03, $08, $08
		BL    $00, $00, _FOO
		ORR   $0a, $02, $02
		ORR   $02, $0a, $0a
		LEAVE
		LEAVE
		times 4096 - ($ - $$) db 0
_stack:
