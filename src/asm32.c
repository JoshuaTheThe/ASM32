
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<asm32.h>

// TODO!(make the upper 4 bits unaffected by address manipulation),,, or not
// it doesn't really make it unsafe

void asm32_interrupt(asm32_t *const cpu, memory *mem, uint32_t code, bool *const error)
{
        const virtual old_rp = cpu->rp;
        if (error && *error) { return; }
        cpu->rp = seg_off(0x00, 0x00C0);
        const virtual pr13 = lea_reg(cpu, 0x0D);
        const virtual pr14 = lea_reg(cpu, 0x0E);
        const virtual new_ppc = lea_reg(cpu, 0x0F);
        asm32_write_word(cpu, mem, pr13, code, error);
        if (error && *error) { return; }
        asm32_write_word(cpu, mem, pr14, old_rp, error);
        if (error && *error) { return; }
        asm32_write_word(cpu, mem, new_ppc, seg_off(0x00, 0x0000), error);
}

bool asm32_is_user_program(asm32_t const *const cpu, memory *mem, bool *const error)
{
        const virtual ppc = lea_reg(cpu, 0x0F);
        const uint32_t pc = asm32_read_word(cpu, mem, ppc, error);
        if (error && *error) { return true; }
        return cpu->seg[asm32_segment_of_address(cpu, pc)].is_user;
}

uint32_t asm32_segment_of_address(asm32_t const *const cpu, virtual address)
{
        (void)cpu;
        const uint8_t primary_segment = (address >> 28) & 0xF;
        return primary_segment;
}

uint32_t asm32_offset_of_address(asm32_t const *const cpu, virtual address)
{
        (void)cpu;
        const uint32_t offset         = address & 0x0FFFFFFF;
        return offset;
}

uint32_t asm32_translate_to_physical(asm32_t const *const cpu, virtual address, bool *const error)
{
        const uint32_t offset          = asm32_offset_of_address(cpu, address);
        const uint8_t  primary_segment = asm32_segment_of_address(cpu, address);
        if (offset > cpu->seg[primary_segment].len)
        {
                if (error)
                        *error = true;
                return 0;
        }

        return cpu->seg[primary_segment].base + offset;
}

uint32_t asm32_read_byte(asm32_t const *const cpu, memory *mem, virtual address, bool *const error)
{
        const uint32_t physical = asm32_translate_to_physical(cpu, address, error);
        if (error && *error) return 0;
        if (physical >= 0xFFFFFF00) return ((uint8_t *)&cpu->seg)[physical % sizeof(cpu->seg)];
        if (physical >= mem->len) return 0;
        return (uint32_t)mem->mem[physical];
}

uint32_t asm32_write_byte(asm32_t const *const cpu, memory *mem, virtual address, uint8_t value, bool *const error)
{
        const uint32_t physical = asm32_translate_to_physical(cpu, address, error);
        if (error && *error) return 0;
        if (physical >= 0xFFFFFF00)
        {
                ((uint8_t *)&cpu->seg)[physical % sizeof(cpu->seg)] = value;
                return value;
        }
        if (physical >= mem->len) return 0;
        return mem->mem[physical] = value;
}

uint32_t asm32_read_word(asm32_t const *const cpu, memory *mem, virtual address, bool *const error)
{
        uint32_t word = 0, i;
        for (i = 0; i < 4; ++i)
        {
                word |= asm32_read_byte(cpu, mem, address+i, error) << (i * 8);
                if (error && *error) return 0;
        }

        return word;
}

uint32_t asm32_write_word(asm32_t const *const cpu, memory *mem, virtual address, uint32_t value, bool *const error)
{
        uint32_t i;
        for (i = 0; i < 4; ++i)
        {
                asm32_write_byte(cpu, mem, address+i, value, error);
                value = (value >> 8);
                if (error && *error) return 0;
        }

        return value;
}

bool asm32_reset(asm32_t *const cpu, memory *mem)
{
        bool error = false;
        memset(cpu->seg, 0, sizeof(cpu->seg));
        cpu->seg[0].base    = 0x00000000;
        cpu->seg[0].len     = 0x7FFFFFFF;
        cpu->seg[0].is_user = false;
        cpu->seg[1].base    = 0xFFFFFF00;
        cpu->seg[1].len     = 0x10 * 0x08;
        cpu->seg[1].is_user = false;
        cpu->rp             = 0x000000C0;
        asm32_write_word(cpu, mem, cpu->rp + 15 * 4, 0x00000000, &error);
        return error;
}
