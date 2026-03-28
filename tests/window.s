        include "macros.s"
        _stack equ 0x1000
_start: LDI     $00, _stack
        BL      $00,$00,_foo
        HLT
PROCEDURE _foo
        INT i
        INT j
        LDI     $02,$1
        STW     $02,$00,j
        LDW     $02,$00,j
        DESTROY i
        DESTROY j
        LEAVE