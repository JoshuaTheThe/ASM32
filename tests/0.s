        include "macros.s"
_start: XOR     $04, $04, $04
        LDR     $05, $04
        LDI     $03, $FF
        LDI     $02, $01
lp:     ADD     $05,$05,$02
        SUB     $03,$03,$02
        BGRE    $03,$04,lp
        LDR     $0E,$02