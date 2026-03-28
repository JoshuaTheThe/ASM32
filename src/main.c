
#include<stdio.h>
#include<stdlib.h>
#include<asm32.h>

uint8_t mm[8192];

int main(void)
{
        bool error;
        asm32_t asm32 = {0};
        memory  mem = {0};
        mem.len = 8192;
        mem.mem = &mm[0];
        error = asm32_reset(&asm32,&mem);
        if (error)
        {
                fprintf(stderr, "An error occurred\n");
                exit(1);
        }
        asm32_interrupt(&asm32, &mem, 0x00, &error);
        if (error)
        {
                fprintf(stderr, "An error occurred\n");
                exit(1);
        }
}
