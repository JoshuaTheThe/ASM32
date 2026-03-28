; ASM32 macros for FASM

macro CTX ra {
        db $00 or (ra shl 4)
}

macro CTS ra {
        db $01 or (ra shl 4)
}

macro LDW ra, rb, i {
        db $02 or (ra shl 4)
        db rb   or ((i and $F) shl 4)
        db ((i shr 4) and $7F) or $80
}

macro LDB ra, rb, i {
        db $02 or (ra shl 4)
        db rb   or ((i and $F) shl 4)
        db ((i shr 4) and $7F)
}

macro STW ra, rb, i {
        db $03 or (ra shl 4)
        db rb   or ((i and $F) shl 4)
        db ((i shr 4) and $7F) or $80
}

macro STB ra, rb, i {
        db $03 or (ra shl 4)
        db rb   or ((i and $F) shl 4)
        db ((i shr 4) and $7F)
}

; B format: [4 opcode][4 Ra][4 Rb][3 Cond][1 Link][16 i] = 4 bytes
; byte 0: opcode | Ra<<4
; byte 1: Rb     | Cond<<4 | Link<<7
; byte 2: i[7:0]
; byte 3: i[15:8]

macro B ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (7 shl 4)
        dw (i-$+2) and $FFFF
}

macro BEQ ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (0 shl 4)
        dw (i - $ + 2) and $FFFF
}

macro BNEQ ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (1 shl 4)
        dw (i - $ + 2) and $FFFF
}

macro BLES ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (2 shl 4)
        dw (i - $ + 2) and $FFFF
}

macro BGREEQ ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (3 shl 4)
        dw (i - $ + 2) and $FFFF
}

macro BGRE ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (4 shl 4)
        dw (i - $ + 2) and $FFFF
}

macro BLESEQ ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (5 shl 4)
        dw (i - $ + 2) and $FFFF
}

macro BABOVE ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (6 shl 4)
        dw (i - $ + 2) and $FFFF
}

; linked variants
macro BL ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (7 shl 4) or $80
        dw (i - $ + 2) and $FFFF
}

macro BLEQ ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (0 shl 4) or $80
        dw (i - $ + 2) and $FFFF
}

macro BLNEQ ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (1 shl 4) or $80
        dw (i - $ + 2) and $FFFF
}

macro BLLES ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (2 shl 4) or $80
        dw (i - $ + 2) and $FFFF
}

macro BLGREEQ ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (3 shl 4) or $80
        dw (i - $ + 2) and $FFFF
}

macro BLGRE ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (4 shl 4) or $80
        dw (i - $ + 2) and $FFFF
}

macro BLLESEQ ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (5 shl 4) or $80
        dw (i - $ + 2) and $FFFF
}

macro BLABOVE ra, rb, i {
        db $04 or (ra shl 4)
        db rb  or (6 shl 4) or $80
        dw (i - $ + 2) and $FFFF
}

; LNK format: [4 opcode][4 Ra][4 Rb][3 Cond][1 unused] = 2 bytes
; byte 0: opcode | Ra<<4
; byte 1: Rb     | Cond<<4

macro LNK ra, rb {
        db $05 or (ra shl 4)
        db rb  or (7 shl 4)
}

macro LNKEQ ra, rb {
        db $05 or (ra shl 4)
        db rb  or (0 shl 4)
}

macro LNKNEQ ra, rb {
        db $05 or (ra shl 4)
        db rb  or (1 shl 4)
}

macro LNKLES ra, rb {
        db $05 or (ra shl 4)
        db rb  or (2 shl 4)
}

macro LNKGREEQ ra, rb {
        db $05 or (ra shl 4)
        db rb  or (3 shl 4)
}

macro LNKGRE ra, rb {
        db $05 or (ra shl 4)
        db rb  or (4 shl 4)
}

macro LNKLESEQ ra, rb {
        db $05 or (ra shl 4)
        db rb  or (5 shl 4)
}

macro LNKABOVE ra, rb {
        db $05 or (ra shl 4)
        db rb  or (6 shl 4)
}

macro PUL ra {
        db $06 or (ra shl 4)
}

macro PUS ra {
        db $07 or (ra shl 4)
}

; ALU: [4 opcode][4 Ra][4 Rb][4 Rc] = 2 bytes
; byte 0: opcode | Ra<<4
; byte 1: Rb     | Rc<<4

macro ADD ra, rb, rc {
        db $08 or (ra shl 4)
        db rb  or (rc shl 4)
}

macro SUB ra, rb, rc {
        db $09 or (ra shl 4)
        db rb  or (rc shl 4)
}

macro XOR ra, rb, rc {
        db $0A or (ra shl 4)
        db rb  or (rc shl 4)
}

macro ORR ra, rb, rc {
        db $0B or (ra shl 4)
        db rb  or (rc shl 4)
}

macro AND ra, rb, rc {
        db $0C or (ra shl 4)
        db rb  or (rc shl 4)
}

macro ROL ra, rb, rc {
        db $0D or (ra shl 4)
        db rb  or (rc shl 4)
}

; LDI: [4 opcode][4 Ra][32 i] = 5 bytes
macro LDI ra, i {
        db $0F or (ra shl 4)
        dd i
}

macro HLT {
        db $0E, $00
}

; pseudo-instructions
macro LDR ra, rb {
        ORR ra, rb, rb
}

macro ENTER {
        PUS     $0C
        PUS     $0D
        LDI     $0D, $40
        AND     $0C, $00, $00
        ADD     $00, $00, $0D
        ;       set new sp
        STW     $00, $0C, $00
        ;       set new pc = body
        LDI     $0D, .body
        STW     $0D, $0C, $3C
        ;       set parent window
        CTX     $0D
        STW     $0D, $0C, $38
        ;       clone arguments
        STW     $03, $0C, ($4 * $3)
        STW     $04, $0C, ($4 * $4)
        STW     $05, $0C, ($4 * $5)
        STW     $06, $0C, ($4 * $6)
        ;       switch
        CTS     $0C
        PUL     $0D
        PUL     $0C
        LNK     $00,$00
.body:
}

macro LEAVE {
        ;       save result
        STW     $02, $0E, $08
        CTS     $0E
}
