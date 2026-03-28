
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<asm32.h>

uint8_t mm[8192] = {0};

int main(int argc, char **argv)
{
        bool error;
        asm32_t asm32 = {0};
        memory  mem = {0};
        memset(mm, 0, 8192);
        mem.len = 8192;
        mem.mem = &mm[0];
        if (argc == 2)
        {
                FILE *fp = fopen(argv[1], "r");
                if (!fp) return -1;
                fread(&mm[0], 1, 8192, fp);
                fclose(fp);
        }

        error = asm32_reset(&asm32,&mem);
        if (error)
        {
                fprintf(stderr, "An error occurred\n");
                exit(1);
        }

        do
        {
                asm32_execute(&asm32, &mem, &error);
                asm32_dump(&asm32, &mem, &error);
                if (error)
                {
                        asm32_interrupt(&asm32, &mem, 0x00, &error);
                        break;
                }
        } while(!asm32.halt);
}
