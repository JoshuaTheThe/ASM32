
#include <lc/lc.h>

symbol symbols[MAX_SYMBOLS] = {0};
bool registers[16] = {0};
int register_base_types[16] = {0};
int register_ptr_depth[16] = {0};
int last_register[16] = {0}, last_register_ptr = 1;
int current_function = -1;
int current_function_offset = 0;
char strings[MAX_STRINGS][MAX_IDENTIFIER] = {0};
int string_cnt = 0;

static void lc_create_string(const char *c_str)
{
        strncpy(strings[string_cnt++], c_str, MAX_IDENTIFIER);
}

static int lc_allocate_register(int type, int depth)
{
        for (int i = 8; i < 14; ++i)
                if (!registers[i]) 
                {
                        registers[i] = true;
                        register_base_types[i] = type;
                        register_ptr_depth[i] = depth;
                        last_register[last_register_ptr++] = i;
                        return i;
                }
        assert(0 && "Ran out of registers");
        return -1;
}

static int lc_top(int x)
{
        return last_register[last_register_ptr - (1 + x)];
}

static int lc_pop(void)
{
        assert(last_register_ptr > 0);
        int r = lc_top(0);
        last_register_ptr--;
        return r;
}

static void lc_free_register(int r)
{
        registers[r % 16] = false;
}

static int lc_allocate(void)
{
        for (int i = 0; i < MAX_SYMBOLS; ++i)
        {
                if (!symbols[i].allocated)
                {
                        memset(&symbols[i], 0, sizeof(symbols[0]));
                        symbols[i].allocated = true;
                        return i;
                }
        }

        assert(0 && "Ran out of symbols");
        return -1;
}

static int lc_find(char name[MAX_IDENTIFIER])
{
        for (int i = 0; i < MAX_SYMBOLS; ++i)
        {
                if (!symbols[i].allocated)
                        continue;
                if (!symbols[i].isfunction && symbols[i].as.variable.function != current_function)
                        continue;
                if (!strncmp(name, symbols[i].identifier, MAX_IDENTIFIER))
                        return i;
        }
        assert(0 && "Symbol not found");
        return -1;
}

static void lc_expr(FILE *fp);

static void lc_primary(FILE *fp)
{
        token tok = lc_next(fp);
        switch (tok.type)
        {
                case TOKEN_STRING:
                        {
                                int r = lc_allocate_register(TYPE_INTEGER, 0);
                                printf("\t\tLDI   $%.2x, __STR%.8x\n", r, string_cnt);
                                lc_create_string(tok.identifier);
                        }
                        break;
                case TOKEN_SYMBOL:
                        {
                                int idx = lc_find(tok.identifier);
                                if (symbols[idx].isfunction)
                                {
                                        lc_primary(fp);
                                        for (int dest = 6 - last_register_ptr; dest > 2 && last_register_ptr > 0; --dest)
                                        {
                                                int top = lc_pop();
                                                printf("\t\tORR   $%.2x, $%.2x, $%.2x\n", dest, top, top);
                                        }
                                        printf("\t\tBL    $00, $00, _%s\n", tok.identifier);
                                        while (last_register_ptr > 0)
                                                lc_free_register(lc_pop());
                                        int r = lc_allocate_register(TYPE_INTEGER, 0);
                                        printf("\t\tORR   $%.2x, $02, $02\n", r);
                                }
                                else
                                {
                                        int r = lc_allocate_register(symbols[idx].as.variable.basetype, symbols[idx].as.variable.ptrdepth);
                                        printf("\t\tLD%c   $%.2x, $07, $%.4x\n", symbols[idx].as.variable.basetype == TYPE_INTEGER ||
                                                                               symbols[idx].as.variable.ptrdepth > 0 ? 'W' : 'B', r, symbols[idx].as.variable.offset);
                                }
                                break;
                        }
                case TOKEN_NUMBER:
                        {
                                int i = lc_allocate_register(TYPE_INTEGER, 0);
                                printf("\t\tLDI   $%.2x, $%.8x\n", i, tok.num);
                        }
                        break;
                case TOKEN_LBRACKET:
                        do
                        {
                                lc_expr(fp);
                                if (lc_peek(fp).type == TOKEN_SEMICOLON)
                                {
                                        lc_next(fp);
                                        continue;
                                }
                                break;
                        } while (1);
                        assert(lc_next(fp).type == TOKEN_RBRACKET && "syntax error");
                        break;
        }
}

static void lc_prefix(FILE *fp)
{
        int deref_len = 0;
        while (lc_peek(fp).type == TOKEN_CAR)
        {
                lc_next(fp);
                ++deref_len;
        }
        lc_primary(fp);
        int adr = lc_top(0);
        for (int i = 0; i < deref_len; ++i)
        {
                printf("\t\tLD%c   $%.2x, $%.2x, $00\n",
                        register_base_types[adr] == TYPE_INTEGER ||
                        (register_ptr_depth[adr] - i) > 1 ? 'W' : 'B',
                        adr,adr);
        }
}

static void lc_add(FILE *fp)
{
        lc_prefix(fp);
        while (lc_peek(fp).type == TOKEN_ADD || lc_peek(fp).type == TOKEN_SUB)
        {
                int type = lc_next(fp).type;
                lc_prefix(fp);
                int rgt = lc_pop();
                int lft = lc_top(0);
                if (type == TOKEN_ADD)
                        printf("\t\tADD   $%.2x, $%.2x, $%.2x\n",lft,lft,rgt);
                else
                        printf("\t\tSUB   $%.2x, $%.2x, $%.2x\n",lft,lft,rgt);
                lc_free_register(rgt);
        }
}

static void lc_and(FILE *fp)
{
        lc_add(fp);
        while (lc_peek(fp).type == TOKEN_AND)
        {
                lc_next(fp);
                lc_add(fp);
                int rgt = lc_pop();
                int lft = lc_top(0);
                printf("\t\tAND   $%.2x, $%.2x, $%.2x\n",lft,lft,rgt);
                lc_free_register(rgt);
        }
}

static void lc_xor(FILE *fp)
{
        lc_and(fp);
        while (lc_peek(fp).type == TOKEN_XOR)
        {
                lc_next(fp);
                lc_and(fp);
                int rgt = lc_pop();
                int lft = lc_top(0);
                printf("\t\tXOR   $%.2x, $%.2x, $%.2x\n",lft,lft,rgt);
                lc_free_register(rgt);
        }
}

static void lc_expr(FILE *fp)
{
        lc_xor(fp);
        while (lc_peek(fp).type == TOKEN_OR)
        {
                lc_next(fp);
                lc_xor(fp);
                int rgt = lc_pop();
                int lft = lc_top(0);
                printf("\t\tORR   $%.2x, $%.2x, $%.2x\n",lft,lft,rgt);
                lc_free_register(rgt);
        }
}

static void lc_stmt(FILE *fp)
{
        token tok = lc_peek(fp);
        switch (tok.type)
        {
                case TOKEN_IF:
                        lc_next(fp);
                        lc_expr(fp);
                        lc_free_register(lc_top(0));
                        lc_stmt(fp);
                        if (lc_peek(fp).type == TOKEN_ELSE)
                        {
                                lc_next(fp);
                        }
                        break;
                case TOKEN_BEGIN:
                        lc_next(fp);
                        while (lc_peek(fp).type != TOKEN_END)
                                lc_stmt(fp);
                        assert(lc_next(fp).type == TOKEN_END && "syntax error");
                        break;
                case TOKEN_RETURN:
                        lc_next(fp);
                        lc_expr(fp);
                        lc_free_register(lc_top(0));
                        int top = lc_pop();
                        printf("\t\tORR   $02, $%.2x, $%.2x\n\t\tLEAVE\n",top,top);
                        break;
                case TOKEN_SET:
                        lc_next(fp);
                        {
                                assert(lc_next(fp).type == TOKEN_LBRACKET && "syntax error");
                                token name = lc_next(fp);
                                assert(name.type == TOKEN_SYMBOL && "syntax error");
                                assert(lc_next(fp).type == TOKEN_SEMICOLON && "syntax error");
                                lc_expr(fp);
                                assert(lc_next(fp).type == TOKEN_RBRACKET && "syntax error");
                                int reg = lc_pop();
                                int idx = lc_find(name.identifier);
                                printf("\t\tST%c   $%.2x, $07, $%.4x\n", symbols[idx].as.variable.basetype == TYPE_INTEGER ||
                                        symbols[idx].as.variable.ptrdepth > 0 ? 'W' : 'B', reg, symbols[idx].as.variable.offset);
                                
                                lc_free_register(reg);
                        }
                        break;
                case TOKEN_DEFINE:
                        lc_next(fp);
                        {
                                int ptr_depth = 0;
                                token name = lc_next(fp), type = {0};
                                assert(name.type == TOKEN_SYMBOL && "syntax error");
                                assert(lc_next(fp).type == TOKEN_COLON && "syntax error");
                                while (lc_peek(fp).type == TOKEN_CAR)
                                {
                                        ++ptr_depth;
                                        lc_next(fp);
                                }
                                type = lc_next(fp);
                                assert((type.type == TOKEN_INT || type.type == TOKEN_CHAR) && "syntax error");
                                int idx = lc_allocate();
                                symbols[idx].as.variable.basetype = type.type == TOKEN_INT ? TYPE_INTEGER : TYPE_CHAR;
                                symbols[idx].as.variable.function = current_function;
                                symbols[idx].as.variable.ptrdepth = ptr_depth;
                                symbols[idx].as.variable.offset   = current_function_offset;
                                current_function_offset += 4;
                                memcpy(symbols[idx].identifier, name.identifier, MAX_IDENTIFIER);
                                int reg = lc_allocate_register(TYPE_INTEGER, 0);
                                printf("\t\tLDI   $%.2x, $00000000\n", reg);
                                printf("\t\tPUS   $%.2x\n", reg);
                                lc_free_register(reg);
                        }
                        break;
                default:
                        lc_expr(fp);
                        break;
        }
        while (last_register_ptr > 0)
                lc_free_register(lc_pop());
        assert(lc_next(fp).type == TOKEN_SEMICOLON && "syntax error");
}

static void lc_procedure(FILE *fp)
{
        int me = lc_allocate();
        token name = lc_next(fp), type = {0};
        assert(name.type == TOKEN_SYMBOL && "syntax error");
        printf("_%.*s:\t\tENTER $04\n",name.num,name.identifier);
        assert(lc_next(fp).type == TOKEN_LBRACKET && "syntax error");
        symbols[me].isfunction = true;
        memcpy(symbols[me].identifier, name.identifier, MAX_IDENTIFIER);
        current_function = me;
        current_function_offset = 0;
        do
        {
                if (lc_peek(fp).type == TOKEN_RBRACKET) break;
                int ptr_depth = 0;
                name = lc_next(fp);
                assert(name.type == TOKEN_SYMBOL && "syntax error");
                assert(lc_next(fp).type == TOKEN_COLON && "syntax error");
                while (lc_peek(fp).type == TOKEN_CAR)
                {
                        ++ptr_depth;
                        lc_next(fp);
                }
                type = lc_next(fp);
                assert((type.type == TOKEN_INT || type.type == TOKEN_CHAR) && "syntax error");
                int idx = lc_allocate();
                symbols[idx].as.variable.basetype = type.type == TOKEN_INT ? TYPE_INTEGER : TYPE_CHAR;
                symbols[idx].as.variable.function = current_function;
                symbols[idx].as.variable.ptrdepth = ptr_depth;
                symbols[idx].as.variable.offset   = current_function_offset;
                current_function_offset += 4;
                memcpy(symbols[idx].identifier, name.identifier, MAX_IDENTIFIER);
                symbols[me].as.function.argument_indexes[symbols[me].as.function.argument_count++] = idx;
                if (lc_peek(fp).type == TOKEN_SEMICOLON)
                {
                        lc_next(fp);
                        continue;
                }
                break;
        } while (symbols[me].as.function.argument_count < 4);
        assert(lc_next(fp).type == TOKEN_RBRACKET && "syntax error");
        current_function_offset = 4 * 4;
        lc_stmt(fp);
        printf("\t\tLEAVE\n");
}

void lc_program(FILE *fp)
{
        token tok;
        printf("\t\tinclude \"macros.s\"\n");
        printf("_start:\t\tLDI   $00, _stack\n");
        printf("\t\tBL    $00, $00, _MAIN\n");
        printf("\t\tHLT\n");
        do
        {
                tok = lc_next(fp);
                if (tok.type == TOKEN_PROCEDURE)
                {
                        lc_procedure(fp);
                }
                else if (tok.type != TOKEN_EOF)
                {
                        assert(0 && "syntax error");
                }
        } while (tok.type != TOKEN_EOF);

        for (int i = 0; i < string_cnt; ++i)
        {
                printf("__STR%.8x:\n", i);
                printf("\t\tdb $%.2lx\n", strlen(strings[i]));
                for (int j = 0; strings[i][j]; ++j)
                        printf("\t\tdb $%.2x\n", strings[i][j]);
        }
        printf("\t\ttimes 4096 - ($ - $$) db 0\n");
        printf("_stack:\n");
}
