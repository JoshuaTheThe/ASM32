
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<asm32.h>
#include<lc/lc.h>

uint8_t mm[8192] = {0};

int emulate(int argc, char **argv)
{
        bool error;
        asm32_t asm32 = {0};
        memory  mem = {0};
        memset(mm, 0, 8192);
        mem.len = 8192;
        mem.mem = &mm[0];
        if (argc == 3)
        {
                FILE *fp = fopen(argv[2], "r");
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
                if (error)
                {
                        asm32_interrupt(&asm32, &mem, 0x00, &error);
                        break;
                }

                if (mem.mem[8192-4])
                {
                        printf("%c",mem.mem[8192-4]);
                        fflush(stdout);
                        mem.mem[8192-4] = 0;
                }
        } while(!asm32.halt);
        return 0;
}

int main(int argc, char **argv)
{
        if (argc >= 2 && !strncmp(argv[1], "-r", 256))
                return emulate(argc, argv);
        else if (argc >= 3 && !strncmp(argv[1], "-c", 256))
        {
                FILE *fin = fopen(argv[2], "r");
                if (!fin) return -1;
                lc_program(fin);
                fclose(fin);
        }
}
