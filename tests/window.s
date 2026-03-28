        include "macros.s"
        _stack equ 0x1000
_start: LDI     $00, _stack
        BL      $00,$00,_foo
        HLT
_foo:
        LDI     $0D, $40
        AND     $0C, $00, $00
        ADD     $00, $00, $0D
        ;       set new sp
        STW     $0C, $00, $00
        ;       set new pc = body
        LDI     $0D, body
        STW     $0C, $0D, $3C
        ;       set parent window
        CTX     $0D
        STW     $0C, $0D, $34
        ;       switch
        CTS     $0C
        HLT
        LNK     $00,$00
body:   HLT
        CTS     $0E