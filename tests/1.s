_FOO:		ENTER $00
		LDW   $08,$07,$0000
		LDW   $09,$07,$0004
		ADD   $08,$08,$09
		ORR   $02,$08,$08
		B     $00,$00,.exit
		LEAVE
_MAIN:		ENTER $00
		LDI   $08,$00000001
		LDI   $09,$00000002
		ORR   $06,$09,$09
		ORR   $05,$08,$08
		ORR   $04,$00,$00
		BL    FOO
		ORR   $0a,$02,$02
		ORR   $02,$0a,$0a
		B     $00,$00,.exit
		LEAVE
