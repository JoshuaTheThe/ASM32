
#ifndef ASM32_H
#define ASM32_H

// if you use a function, check if error

#include<stdint.h>
#include<stdbool.h>

#define lea_reg(cpu,idx) ((cpu)->rp + ((uint32_t)(idx) * 4))
#define seg_off(seg,off) (((seg) << 28) | (off))

typedef uint32_t virtual;

typedef struct
{
        uint8_t *mem;
        uint32_t len;
} memory;

typedef struct
{
        uint32_t base, len : 31, is_user : 1;
} asm32_segment_t;

typedef struct
{
        asm32_segment_t seg[16];
        uint32_t rp;
        bool halt;
} asm32_t;

bool     asm32_is_user_program(asm32_t const *const cpu, memory *mem, bool *const error);
uint32_t asm32_segment_of_address(asm32_t const *const cpu, virtual address);
uint32_t asm32_offset_of_address(asm32_t const *const cpu, virtual address);
uint32_t asm32_translate_to_physical(asm32_t const *const cpu, virtual address, bool *const error);
uint32_t asm32_read_byte(asm32_t const *const cpu, memory *mem, virtual address, bool *const error);
uint32_t asm32_write_byte(asm32_t const *const cpu, memory *mem, virtual address, uint8_t value, bool *const error);
uint32_t asm32_read_word(asm32_t const *const cpu, memory *mem, virtual address, bool *const error);
uint32_t asm32_write_word(asm32_t const *const cpu, memory *mem, virtual address, uint32_t value, bool *const error);
uint64_t asm32_read_instruction(asm32_t const *const cpu, memory *mem, virtual address, bool *const error);
void     asm32_interrupt(asm32_t *const cpu, memory *mem, uint32_t code, bool *const error);
bool     asm32_reset(asm32_t *const cpu, memory *mem);
void     asm32_execute(asm32_t *const cpu, memory *mem, bool *const error);
uint32_t asm32_read_register(asm32_t *const cpu, memory *mem, uint8_t index, bool *const error);
uint32_t asm32_write_register(asm32_t *const cpu, memory *mem, uint8_t index, uint32_t value, bool *const error);
void     asm32_dump(asm32_t *const cpu, memory *mem, bool *const error);

#endif
