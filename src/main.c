
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<asm32.h>

uint8_t mm[8192] = {0};

int main(void)
{
        bool error;
        asm32_t asm32 = {0};
        memory  mem = {0};
        memset(mm, 0, 8192);
        mem.len = 8192;
        mem.mem = &mm[0];
        mm[0] = 0xEF; // LDI 14,
        mm[1] = 0x0F; // 0x0F
        error = asm32_reset(&asm32,&mem);
        if (error)
        {
                fprintf(stderr, "An error occurred\n");
                exit(1);
        }

        do
        {
                asm32_execute(&asm32, &mem, &error);
                if (error)
                {
                        asm32_interrupt(&asm32, &mem, 0x00, &error);
                        break;
                }
        } while(0);
        asm32_dump(&asm32, &mem, &error);
}
