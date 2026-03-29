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
_MAIN:		ENTER $04
		LDI   $08, __STR00000000
		LDI   $09, $00000001
		ADD   $08, $08, $09
		LDB   $08, $08, $00
		ORR   $03, $08, $08
		BL    $00, $00, _PUTCHAR
		ORR   $09, $02, $02
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
