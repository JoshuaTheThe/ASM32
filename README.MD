
# ASM32 Architecture (Aderyn's Simple Memory Architecture / 32)
- simple 32 bit memory to memory ISA
- no flags
- little endian

# Registers
| Name | Description | Bits | Saved By |
| - | - | - | - |
| `R0` | note: Stack Pointer | 32 | Callee |
| `R1` | note: Link Register | 32 | Callee |
| `R2` | note: By Convention, return value | 32 | Callee |
| `R3-R6` | note: By Convention, Argument | 32 | Callee |
| `R7` | note: By Convention, Data Pointer | 32 | Callee |
| `R8-R13` | note: By Convention, Scratch | 32 | Callee |
| `R14` | note: By Convention, Old Window | 32 | Callee |
| `R15` | note: Program Counter | 32 | Caller |
| `R[0-15]` | Register File Entry | 32 |
| `RP` | Register File Pointer | 32 |

# Addressing
- top 4 bits are Segment

## Segmentation
- lookup index in an array of such
- please note, segment list is mapped as physical `$FFFFFF00` *always*

```c
struct  SEGMENT
{
        uint32_t        PHYS_BASE;
        uint32_t        PHYS_LENG : 31;
        uint8_t         IS_USER : 1;
};
```

## Interrupts
`IAMNOW $00:C0` on raise, set `R13` to the code and `R14` to the old `RP`.
`R13:=code,R14:=RP,RP:=0H:000000C0H`

## Protection
if we are a user program (PC in a USER Segment), do `IAMNOW $00:C0` with interrupt $01 on violation

# Boot up
On bootup, the Register File Pointer is set to `$C0`, and the Program Counter is set to `$00`

# Operation Codes
| Opcode | Mnemonic | Function | Format | Bytes |
|-|-|-|-|-|
| `0000` | `CTX` | `Ra := RP` | `[4 bit Opcode][4 bit Ra]` 1 | 1 |
| `0001` | `CTS` | `RP := Ra` | `[4 bit Opcode][4 bit Ra]` 2 | 1 |
| `0010` | `LDW`,`LDB`  | `Ra := *Rb+i` | `[4 bit Opcode][4 bit Ra][4 bit Rb][11 bit i][1 bit, Word or Byte]` | 3 |
| `0011` | `STW`,`STB`  | `*Rb+i := Ra` | `[4 bit Opcode][4 bit Ra][4 bit Rb][11 bit i][1 bit, Word or Byte]` | 3 |
| `0100` | `B`,`BEQ`,`BLES`,`BGRE`,`BL`,`BLEQ`,`BLLES`,`BLGRE`,...  | `IF ConditionTrue: { R1 := Link ? next() : R1 R15 := R15 + sign_extend(i) }` | `[4 bit Opcode][4 bit Ra][4 bit Rb][3 bit Condition Code][1 bit Link][16 bit i]` | 4 |
| `0101` | `LNK`,`LNKEQ`,`LNKLES`,`LNKGRE`,... | `IF ConditionTrue: R15 := R1` | `[4 bit Opcode][4 bit Ra][4 bit Rb][3 bit Condition Function][1 bit Unused]` | 2 |
| `0110` | `PUL`  | `R0 := R0 - 04, Ra := *R0` | `[4 bit Opcode][4 bit Ra]` | 1 |
| `0111` | `PUS`  | `*R0 := Ra, R0 := R0 + 04` | `[4 bit Opcode][4 bit Ra]` | 1 |
| `1000` | `ADD`  | `Ra := Rb + Rc` | `[4 bit Opcode][4 bit Ra][4 bit Rb][4 bit Rc]` | 2 |
| `1001` | `SUB`  | `Ra := Rb + ~Rc + 1` | `[4 bit Opcode][4 bit Ra][4 bit Rb][4 bit Rc]` | 2 |
| `1010` | `XOR`  | `Ra := Rb ^ Rc` | `[4 bit Opcode][4 bit Ra][4 bit Rb][4 bit Rc]` | 2 |
| `1011` | `ORR`  | `Ra := Rb \| Rc` | `[4 bit Opcode][4 bit Ra][4 bit Rb][4 bit Rc]` | 2 |
| `1100` | `AND`  | `Ra := Rb & Rc` | `[4 bit Opcode][4 bit Ra][4 bit Rb][4 bit Rc]` | 2 |
| `1101` | `ROL`  | `Ra := Rb <<< Rc` | `[4 bit Opcode][4 bit Ra][4 bit Rb][4 bit Rc]` | 2 |
| `1110` | `EXT`  | Reserved | `[4 bit Opcode][12 bits unused]` | 2* |
| `1111` | `LDI`  | `Ra := i` | `[4 bit Opcode][4 bit Ra][32 bit i]` | 5 |

- `Bxx` doesn't affect top 4 bits, nor does `LINKxx`
- `WordOrByte ? Word : Byte;`

1. `get ConTeXt`
2. `ConText Switch`

## Condition Codes
| Index | Condition | Signed? |
| `000` | `EQ` | N/A |
| `001` | `NEQ` | N/A |
| `010` | `LES` | T |
| `011` | `GREEQ` | T |
| `100` | `GRE` | T |
| `101` | `LESEQ` | T |
| `110` | `BELOW` | F |
| `111` | `ALWAYS` | N/A |