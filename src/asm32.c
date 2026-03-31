
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<asm32.h>

#define DBG

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

 // largest op is 5 bytes, so we fetch whole for performance
uint64_t asm32_read_instruction(asm32_t const *const cpu, memory *mem, virtual address, bool *const error)
{
        uint64_t word = 0, i;
        for (i = 0; i < 5; ++i)
        {
                word |= (uint64_t)asm32_read_byte(cpu, mem, address+i, error) << (i * 8);
                if (error && *error) return 0;
        }

        return word;
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

uint32_t asm32_read_register(asm32_t *const cpu, memory *mem, uint8_t index, bool *const error)
{
        const virtual virt = lea_reg(cpu, index & 0xF);
        const uint32_t value = asm32_read_word(cpu, mem, virt, error);
        return value;
}

uint32_t asm32_write_register(asm32_t *const cpu, memory *mem, uint8_t index, uint32_t value, bool *const error)
{
        const virtual virt = lea_reg(cpu, index & 0xF);
        return asm32_write_word(cpu, mem, virt, value, error);
}

uint32_t asm32_sign_extend_11(uint32_t val)
{
        if ((val >> 10) & 1)
                return val | 0xFFFFF800;
        return val & 0x000007FF;
}

uint32_t asm32_sign_extend_16(uint32_t val)
{
        if ((val >> 15) & 1)
                return val | 0xFFFF0000;
        return val & 0x0000FFFF;
}

bool asm32_is_condition_true(asm32_t *const cpu, memory *mem, uint8_t Ra, uint8_t Rb, uint8_t Condition, bool *const error)
{
        int32_t left  = asm32_read_register(cpu, mem, Ra, error);
        if (error && *error) return false;
        int32_t right = asm32_read_register(cpu, mem, Rb, error);
        if (error && *error) return false;
        switch (Condition)
        {
                case 0: return left == right;
                case 1: return left != right;
                case 2: return left <  right;
                case 3: return left >= right;
                case 4: return left >  right;
                case 5: return left <= right;
                case 6: return (uint32_t)left < (uint32_t)right;
                default:
                case 7: return true;
        }
}

void asm32_execute(asm32_t *const cpu, memory *mem, bool *const error)
{
        static const uint8_t required_bytes[16] = {1,1,3,3,4,2,1,1,2,2,2,2,2,2,2,5};
        const virtual pc = asm32_read_register(cpu, mem, 0x0F, error);
        if (error && *error) return;
        const uint64_t instruction = asm32_read_instruction(cpu, mem, pc, error);
        if (error && *error) return;
        asm32_write_register(cpu, mem, 0x0F, pc + required_bytes[instruction & 0x0F], error);

        const uint8_t  Ra         = (instruction >>  4) & 0xF;
        const uint8_t  Rb         = (instruction >>  8) & 0xF;
        const uint8_t  Rc         = (instruction >> 12) & 0xF;
        const bool     WordOrByte = (instruction >> 23) & 0x01;
        const uint32_t I11        = asm32_sign_extend_11((instruction >> 12) & 0x07FF);
        const uint32_t I16        = asm32_sign_extend_16((instruction >> 16) & 0xFFFF);
        const uint32_t I32        = (instruction >>  8) & 0xFFFFFFFF;
        const uint32_t Cond       = (instruction >> 12) & 0x07;
        const uint32_t Link       = (instruction >> 15) & 0x01;
        const uint8_t  Opcode     = instruction & 0xF;
#ifdef DBG
        fprintf(stderr, "Ra: %.2x ", Ra);
        fprintf(stderr, "Rb: %.2x ", Rb);
        fprintf(stderr, "Rc: %.2x ", Rc);
        fprintf(stderr, "Sz: %.2x ", WordOrByte * 4);
        fprintf(stderr, "11: %.4x ", I11 & 0x07FF);
        fprintf(stderr, "16: %.4x ", I16 & 0xFFFF);
        fprintf(stderr, "32: %.8x ", I32);
        fprintf(stderr, "CON:%.1x ", Cond);
        fprintf(stderr, "LNK:%.1x ", Link);
        fprintf(stderr, "OPC:%.8x ", Opcode);
        fprintf(stderr, "AT :%.8x ", pc);
        fprintf(stderr, "INS:%.16lx ", instruction);
#endif
        uint32_t Temp=0,Temp2     = 0;
        uint32_t Base             = 0;

        switch (Opcode)
        {
        case 0x00:      // CTX
                asm32_write_register(cpu, mem, Ra, cpu->rp, error);
#ifdef DBG
                fprintf(stderr, "CTX\n");
#endif
                break;
        case 0x01:      // CTS
                cpu->rp = asm32_read_register(cpu, mem, Ra, error);
#ifdef DBG
                fprintf(stderr, "CTS\n");
#endif
                break;
        case 0x2:       // LDW/LDB
                Base = asm32_read_register(cpu, mem, Rb, error);
                if (error && *error) return;
                Temp = (WordOrByte) ? asm32_read_word(cpu, mem, (uint32_t)(*(int32_t *)&Base + *(int32_t *)&I11), error) :
                                      asm32_read_byte(cpu, mem, (uint32_t)(*(int32_t *)&Base + *(int32_t *)&I11), error);
                if (error && *error) return;
                asm32_write_register(cpu, mem, Ra, Temp, error);
#ifdef DBG
                fprintf(stderr, "LDX\n");
#endif
                break;
        case 0x3:       // STW/STB
                Base = asm32_read_register(cpu, mem, Rb, error);
                if (error && *error) return;
                Temp = asm32_read_register(cpu, mem, Ra, error);
                if (error && *error) return;
                (WordOrByte) ? asm32_write_word(cpu, mem, (uint32_t)(*(int32_t *)&Base + *(int32_t *)&I11), Temp, error) :
                               asm32_write_byte(cpu, mem, (uint32_t)(*(int32_t *)&Base + *(int32_t *)&I11), Temp, error);
#ifdef DBG
                fprintf(stderr, "STX\n");
#endif
                break;
        case 0x4:       // Bxx
                if (asm32_is_condition_true(cpu, mem, Ra, Rb, Cond, error))
                {
                        if (Link)
                        {
                                asm32_write_register(cpu, mem, 0x01, pc + required_bytes[instruction & 0x0F], error);
                                if (error && *error) return;
                        }

                        const virtual new_pc = (pc & 0xF0000000) | ((uint32_t)((int32_t)(pc & 0x0FFFFFFF) + *(int32_t *)&I16) & 0x0FFFFFFF);
                        asm32_write_register(cpu, mem, 0x0F, new_pc, error);
                }
#ifdef DBG
                fprintf(stderr, "BXX\n");
#endif
                break;
        case 0x5:       // LNKXX
                if (asm32_is_condition_true(cpu, mem, Ra, Rb, Cond, error))
                {
                        Temp = asm32_read_register(cpu, mem, 0x01, error);
                        if (error && *error) return;
                        asm32_write_register(cpu, mem, 0x0F, Temp, error);
                }
#ifdef DBG
                fprintf(stderr, "LXX\n");
#endif
                break;
        case 0x6:       // PUL
                Temp = asm32_read_register(cpu, mem, 0x00, error);
                if (error && *error) return;
                Temp -= 4;
                asm32_write_register(cpu, mem, 0x00, Temp, error);
                if (error && *error) return;
                Temp = asm32_read_word(cpu, mem, Temp, error);
                if (error && *error) return;
                asm32_write_register(cpu, mem, Ra, Temp, error);
#ifdef DBG
                fprintf(stderr, "PUL\n");
#endif
                break;
        case 0x7:       // PUS
                Temp = asm32_read_register(cpu, mem, 0x00, error);
                if (error && *error) return;
                Base = asm32_read_register(cpu, mem, Ra, error);
                if (error && *error) return;
                asm32_write_word(cpu, mem, Temp, Base, error);
                if (error && *error) return;
                Temp += 4;
                asm32_write_register(cpu, mem, 0x00, Temp, error);
#ifdef DBG
                fprintf(stderr, "PUS\n");
#endif
                break;
        case 0x8:       // ADD
                Temp  = asm32_read_register(cpu, mem, Rb, error);
                if (error && *error) return;
                Temp2 = asm32_read_register(cpu, mem, Rc, error);
                if (error && *error) return;
                Temp  = Temp + Temp2;
                asm32_write_register(cpu, mem, Ra, Temp, error);
#ifdef DBG
                fprintf(stderr, "ADD\n");
#endif
                break;
        case 0x9:       // SUB
                Temp  = asm32_read_register(cpu, mem, Rb, error);
                if (error && *error) return;
                Temp2 = asm32_read_register(cpu, mem, Rc, error);
                if (error && *error) return;
                Temp  = Temp - Temp2;
                asm32_write_register(cpu, mem, Ra, Temp, error);
#ifdef DBG
                fprintf(stderr, "SUB\n");
#endif
                break;
        case 0xA:       // XOR
                Temp  = asm32_read_register(cpu, mem, Rb, error);
                if (error && *error) return;
                Temp2 = asm32_read_register(cpu, mem, Rc, error);
                if (error && *error) return;
                Temp  = Temp ^ Temp2;
                asm32_write_register(cpu, mem, Ra, Temp, error);
#ifdef DBG
                fprintf(stderr, "XOR\n");
#endif
                break;
        case 0xB:       // ORR
                Temp  = asm32_read_register(cpu, mem, Rb, error);
                if (error && *error) return;
                Temp2 = asm32_read_register(cpu, mem, Rc, error);
                if (error && *error) return;
                Temp  = Temp | Temp2;
                asm32_write_register(cpu, mem, Ra, Temp, error);
#ifdef DBG
                fprintf(stderr, "ORR\n");
#endif
                break;
        case 0xC:       // AND
                Temp  = asm32_read_register(cpu, mem, Rb, error);
                if (error && *error) return;
                Temp2 = asm32_read_register(cpu, mem, Rc, error);
                if (error && *error) return;
                Temp  = Temp & Temp2;
                asm32_write_register(cpu, mem, Ra, Temp, error);
#ifdef DBG
                fprintf(stderr, "AND\n");
#endif
                break;
        case 0xD:       // ROL
                Temp  = asm32_read_register(cpu, mem, Rb, error);
                if (error && *error) return;
                Temp2 = asm32_read_register(cpu, mem, Rc, error);
                if (error && *error) return;
                Temp2 = Temp2 & 0x1F;
                Temp  = (Temp << Temp2) | (Temp >> (32 - Temp2));
                asm32_write_register(cpu, mem, Ra, Temp, error);
#ifdef DBG
                fprintf(stderr, "ROL\n");
#endif
                break;
        case 0xE:       // EX
#ifdef DBG
                fprintf(stderr, "EX,HLT\n");
#endif
                cpu->halt = true;
                break;
        case 0xF:       // LDI
#ifdef DBG
                fprintf(stderr, "LDI\n");
#endif
                asm32_write_register(cpu, mem, Ra, I32, error);
                break;
        }
}

void asm32_dump(asm32_t *const cpu, memory *mem, bool *const error)
{
        int i;
        fprintf(stderr,"\n\t");
        for (i = 0; i < 16; ++i)
        {
                uint32_t val = asm32_read_register(cpu, mem, i, error);
                if (error && *error)
                {
                        fprintf(stderr,"r%.2d: ???????? = ???????? ", i);
                }
                else
                {
                        fprintf(stderr,"r%.2d: %.8x = %.8x ", i, lea_reg(cpu, i), val);
                }
                if ((i & 0x03) == 0x03)
                        fprintf(stderr,"\n\t");
        }
        fprintf(stderr," rp: %.8x\n\n", cpu->rp);
}
