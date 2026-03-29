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
		LDI   $08, __STR00000000
		STW   $08, $07, $0010
		LDW   $08, $07, $0010
		ORR   $02, $08, $08
		LEAVE
		LEAVE
__STR00000000:
		db $0e
		db $48
		db $65
		db $6c
		db $6c
		db $6f
		db $2c
		db $20
		db $57
		db $6f
		db $72
		db $6c
		db $64
		db $21
		db $0a
		times 4096 - ($ - $$) db 0
_stack:
