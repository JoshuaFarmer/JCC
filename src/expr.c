#define _expr_
#include "expr.h"
#include <stdarg.h>
void expr();

Variable list;
char *src;
char id[64];
int  tok,num;
int  bpoff=4;
int  use_eax=1;

char    id_stack[256][64];
uint8_t id_stack_ptr = 0;

void PushID()
{
        strcpy(id_stack[id_stack_ptr++],id);
}

void PopID()
{
        strcpy(id,id_stack[--id_stack_ptr]);
}

void ClearID()
{
        memset(id_stack,0,sizeof(id_stack));
}

void skip_comment()
{
        if (*src == '/' && *(src + 1) == '*')
        {
                src += 2;
                while (*src && !(*src == '*' && *(src + 1) == '/'))
                {
                        src++;
                }
                if (*src)
                {
                        src += 2;
                }
        }
}

void skip_whitespace()
{
        while (*src == ' ' || *src == '\t' || *src == '\n') src++;
}

void next()
{
        num=0;
        skip_whitespace();
        skip_comment();
        memset(id,0,sizeof(id));
        if (isalpha(*src))
        {
                int i = 0;
                while (isalnum(*src) && i < 31) {
                        id[i++] = *src++;
                } id[i] = '\0';
                for (int j = 0; j < 31; ++j)
                {
                        if (strcmp(keys[j].text,id)==0)
                        {
                                tok=keys[j].tok;
                                return;
                        }
                }
                tok = TOK_IDE;
                return;
        }
        else if (*src >= '0' && *src <= '9')
        {
                while (*src >= '0' && *src <= '9')
                {
                        num = num * 10 + (*src-'0');
                        ++src;
                }
                tok=TOK_NUM;
                return;
        }
        else if (*src == '!' && *(src+1) == '=')
        {
                src += 2;
                tok = TOK_NEQ;
                return;
        }
        else if (*src == '=' && *(src+1) == '=')
        {
                src += 2;
                tok = TOK_EQ;
                return;
        }

        tok = *src++;
}

void emit(const char *format, ...)
{
        va_list args;
        va_start(args, format);
        vfprintf(fo, format, args);
        va_end(args);
}

void clean()
{
        Variable * x = list.next;
        Variable * prev = NULL;
        while (x != NULL)
        {
                if (prev)
                {
                        free(prev->name);
                        free(prev);
                }
                prev=x;
                x=x->next;
        }
        bpoff=4;
}

Variable * cvar(int size, char * name, int is_const)
{
        fprintf(fo,"\tsub esp,%d\n",size);
        Variable * new = malloc(sizeof(Variable));
        if (!new) exit(2);
        new->con=is_const;
        new->name=strdup(name);
        new->next=list.next;
        new->bpoff=bpoff;
        new->size=size;
        new->assigned=0;
        list.next=new;
        bpoff += size;
        return new;
}

Variable * gvar(const char * name)
{
        Variable * x = list.next;
        while (x != NULL)
        {
                if (strncmp(name,x->name,32)==0)
                {
                        return x;
                }
                x=x->next;
        }

        return NULL;
}

void vars()
{
        Variable * x = list.next;
        while (x != NULL)
        {
                printf("sizeof(%s)==%d,const=%d,bpoff=%d\n",x->name,x->size,x->con,x->bpoff);
                x=x->next;
        }
}

void block()
{
        while (tok != '{' && tok)
                next();
        while (tok != '}' && tok)
                expr();
        next();
        use_eax=1;
}

bool get_var_name()
{
        bool is_ptr = false;
        next();
        if (tok == '*')
        {
                next();
                is_ptr = true;
        }

        PushID();
        return is_ptr;
}

bool is_assignment()
{
        char * tmp = src;
        next();
        src = tmp;
        if (tok == '=')
        {
                return true;
        }
        return false;
}

void skip_til(char x)
{
        while (tok != x && *src && tok) next();
}

bool is_function()
{
        char * tmp = src;
        next();
        src=tmp;
        if (tok == '(')
        {
                return true;
        }
        return false;
}

bool is_function_declaration()
{
        if (!is_function())
        {
                return false;
        }

        char * tmp = src;
        next();
        while (tok != ')')
        {
                next();
        }
        next();
        src = tmp;
        return (tok == '{');
}

void create_arguments()
{
        int c=0;
        int start = 4;
        while (*src && tok && tok != ')')
        {
                if (tok == TOK_IDE)
                {
                        Variable * x = cvar(4, id, c);
                        emit("\tmov eax,[ebp+%d]\n",start);
                        emit("\tmov [ebp-%d],eax\n",x->bpoff);
                        start+=4;
                        c = 0;
                }
                else if (tok == TOK_CONST)
                {
                        c = 1;
                }
                next();
        }
}

void collect_arguments()
{
        int c=0;
        int start = 4;
        while (*src && tok && tok != ')')
        {
                expr();
                emit("\tpush eax\n");
                use_eax=1;
        }
}

#define is_start() (use_eax)

void body()
{
        while (tok != '{') next();
        while (tok != '}') expr();
        next();
        use_eax = 1;
}

void expr()
{
        static int con=0;
        next();
        switch(tok)
        {
                case TOK_NUM:
                {
                        emit("\tmov e%cx,%d\n",(use_eax)?'a':'b',num);
                        use_eax=0;
                } break;

                case TOK_CONST:
                {
                        con=1;
                } break;

                case TOK_INT:
                case TOK_SHORT:
                case TOK_CHAR:
                {
                        int type = tok;
                        bool is_ptr = get_var_name();
                        PopID();
                        if (is_ptr) { cvar(4, id, con); }
                        else
                        {
                                switch (type)
                                {
                                        case TOK_INT:   cvar(4, id, con); break;
                                        case TOK_SHORT: cvar(2, id, con); break;
                                        case TOK_CHAR:  cvar(1, id, con); break;
                                }
                        }
                } break;

                case TOK_EXTERN:
                {
                        next();
                        emit("\textern %s\n",id);
                        skip_til(')');
                } break;

                case TOK_IDE:
                {
                        PushID();
                        if (is_function_declaration())
                        {
                                PopID();
                                char nam[64];
                                strcpy(nam,id);
                                emit("%s:\n",nam);
                                emit("\tpush ebp\n",nam);
                                emit("\tmov ebp,esp\n",nam);
                                next();
                                next();
                                create_arguments();
                                body();
                                emit("%s.exit:\n",nam);
                                emit("\tmov esp,ebp\n");
                                emit("\tpop ebp\n");
                                emit("\tret\n");
                        }
                        else if (is_function())
                        {
                                PopID();
                                char nam[64];
                                strcpy(nam,id);
                                collect_arguments();
                                emit("\tcall %s\n",nam);
                        }
                        else if (!is_assignment())
                        {
                                PopID();
                                Variable * var = gvar(id);
                                if (!var)
                                {
                                        /* TODO - PANIC */
                                        vars();
                                        printf("`%s` not found\n",id);
                                        return;
                                }
                                emit("\tmov e%cx,[ebp-%d]\n",(use_eax)?'a':'b',var->bpoff);
                                PushID();
                        }
                } break;

                case ';':
                {
                        use_eax = 1;
                        return;
                } break;

                case '&':
                {
                        if (is_start())
                        {
                                bool is_ptr = get_var_name();
                                if (is_ptr)
                                {
                                        /* TODO - PANIC */
                                        return;
                                }

                                PopID();
                                Variable * var = gvar(id);
                                if (!var)
                                {
                                        /* TODO - PANIC */
                                        return;
                                }
                                emit("\tlea eax,[ebp-%d] \n",var->bpoff);
                        }
                        else
                        {
                                expr();
                                emit("\tand eax,ebx\n");
                        }
                } break;

                case '=':
                {
                        PopID();
                        char name[64];
                        strcpy(name,id);
                        expr();

                        Variable * var = gvar(name);
                        if (!var || (var->con && var->assigned))
                        {
                                /* TODO - PANIC */
                                return;
                        }

                        emit("\tmov [ebp-%d],eax \n",var->bpoff);
                        var->assigned = true;
                } break;

                case '+':
                {
                        expr();
                        emit("\tadd eax,ebx\n");
                } break;

                case '-':
                {
                        expr();
                        emit("\tsub eax,ebx\n");
                } break;

                case ')': break;
                case '(':
                {
                        while (tok != ')')
                        {
                                expr();
                        }
                } break;
        }
}

void compiler(char * buff)
{
        src=buff;
        while (*src)
        {
                expr();
        }
        clean();
}
