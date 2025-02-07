#define _expr_
#include "expr.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

Variable list;
char *src;
char ident[64];
int  tok,num;
int  bpoff=4;
int  use_eax=1;

char    ident_stack[256][64];
uint8_t ident_stack_ptr = 0;

void PushID()
{
        strcpy(ident_stack[ident_stack_ptr++],ident);
}

void PopID()
{
        strcpy(ident,ident_stack[--ident_stack_ptr]);
}

void ClearID()
{
        memset(ident_stack,0,sizeof(ident_stack));
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
        memset(ident,0,sizeof(ident));
        if (isalpha(*src))
        {
                int i = 0;
                while (isalnum(*src) && i < 31) {
                        ident[i++] = *src++;
                } ident[i] = '\0';
                for (int j = 0; j < 31; ++j)
                {
                        if (strcmp(keys[j].text,ident)==0)
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
                tok = TOK_NEQ;
                return;
        }
        else if (*src == '=' && *(src+1) == '=')
        {
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

void cleanup()
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

Variable * create_Variable(int size, char * name, int is_const)
{
        fprintf(fo,"SUB ESP,%d\n",size);
        Variable * new = malloc(sizeof(Variable));
        if (!new) exit(2);
        new->is_constant=is_const;
        new->name=strdup(name);
        new->next=list.next;
        new->bpoff=bpoff;
        new->size=size;
        new->assigned=0;
        list.next=new;
        bpoff += size;
        return new;
}

Variable * get_Variable(const char * name)
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

void dumpVariables()
{
        Variable * x = list.next;
        while (x != NULL)
        {
                printf("sizeof(%s)==%d,const=%d,bpoff=%d\n",x->name,x->size,x->is_constant,x->bpoff);
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

void expr()
{
        next();
        switch(tok)
        {
                case TOK_NUM:
                {
                        emit("MOV E%cX,%d\n",(use_eax)?'A':'B',num);
                } break;

                case '+':
                {
                        expr();
                        emit("ADD EAX,EBX\n");
                } break;
        }
        use_eax=0;
}
