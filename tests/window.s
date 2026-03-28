        include "macros.s"
        _stack equ 0x1000
_start: LDI     $00, _stack
        BL      $00,$00,_foo
        HLT
_foo:   ENTER
        LDI     $02, 1
        LEAVE