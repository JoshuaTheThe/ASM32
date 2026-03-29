		include "macros.s"
_start:		LDI   $00, _stack
		BL    $00, $00, _MAIN
		HLT
_MAIN:		ENTER $04
		LDI   $08, $00000008
		LDI   $09, $00000008
		ADD   $08, $08, $09
		ORR   $02, $08, $08
		B     $00, $00, .exit
		LEAVE
_stack:
