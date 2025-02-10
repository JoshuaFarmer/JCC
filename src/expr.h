#define _expr_

#ifndef EXPR_H
#define EXPR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

extern FILE * fo;

enum
{
        TOK_AUTO=0x100,
        TOK_BREAK,
        TOK_CASE,
        TOK_CHAR,
        TOK_CONST,
        TOK_CONTINUE,
        TOK_DEFAULT,
        TOK_DO,
        TOK_DOUBLE,
        TOK_ELSE,
        TOK_ENUM,
        TOK_EXTERN,
        TOK_FLOAT,
        TOK_FOR,
        TOK_GOTO,
        TOK_IF,
        TOK_INT,
        TOK_LONG,
        TOK_REGISTER,
        TOK_RETURN,
        TOK_SHORT,
        TOK_SIGNED,
        TOK_SIZEOF,
        TOK_STATIC,
        TOK_STRUCT,
        TOK_SWITCH,
        TOK_TYPEDEF,
        TOK_UNION,
        TOK_UNSIGNED,
        TOK_VOID,
        TOK_WHILE,
        TOK_IDE,
        TOK_EQ,
        TOK_NEQ,
        TOK_NUM,
        TOK_STR,
};

struct KEY
{
        char * text;
        int    tok;
};

typedef struct STRING
{
        char text[64];
        struct STRING * next;
} STRING;

extern char * src;
extern int tok;
void compiler(char *);

typedef struct
{
        uint8_t data[256];
        uint8_t sp;
} Stack;

typedef struct VAR
{
        int size;
        int bpoff;
        bool con;
        bool assigned;
        char * name;
        struct VAR * next;
} Variable;

#ifdef _expr_
struct KEY keys[]=
{
        {.text="auto",TOK_AUTO},
        {.text="break",TOK_BREAK},
        {.text="case",TOK_CASE},
        {.text="char",TOK_CHAR},
        {.text="const",TOK_CONST},
        {.text="continue",TOK_CONTINUE},
        {.text="default",TOK_DEFAULT},
        {.text="do",TOK_DO},
        {.text="double",TOK_DOUBLE},
        {.text="else",TOK_ELSE},
        {.text="enum",TOK_ENUM},
        {.text="extern",TOK_EXTERN},
        {.text="float",TOK_FLOAT},
        {.text="for",TOK_FOR},
        {.text="goto",TOK_GOTO},
        {.text="if",TOK_IF},
        {.text="int",TOK_INT},
        {.text="long",TOK_LONG},
        {.text="register",TOK_REGISTER},
        {.text="return",TOK_RETURN},
        {.text="short",TOK_SHORT},
        {.text="signed",TOK_SIGNED},
        {.text="sizeof",TOK_SIZEOF},
        {.text="static",TOK_STATIC},
        {.text="struct",TOK_STRUCT},
        {.text="switch",TOK_SWITCH},
        {.text="typedef",TOK_TYPEDEF},
        {.text="union",TOK_UNION},
        {.text="unsigned",TOK_UNSIGNED},
        {.text="void",TOK_VOID},
        {.text="while",TOK_WHILE},
};

void spush(Stack * stck, int x)
{
        stck->data[stck->sp++]=x;
}

int spop(Stack * stck)
{
        return stck->data[--stck->sp];
}

int stop(Stack * stck)
{
        return stck->data[stck->sp-1];
}
#endif

#endif

#include <stdarg.h>
void expr();

Variable list;
char *src;
char id[64];
int  tok,num;
int  bpoff=4;
int  use_eax=1;
int  m = 0;

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
        while (*src == ' ' || *src == '\t' || *src == '\n' || *src == '\r') src++;
}

void next()
{
        num=0;
        skip_whitespace();
        skip_comment();
        memset(id,0,sizeof(id));
        if (*src == '\'')
        {
                ++src;
                tok=TOK_NUM;
                num=*src;
                ++src;
                return;
        }
        else if (*src == '"')
        {
                ++src;
                int x = 0;
                while (*src != '"')
                {
                        if (*src == '\\')
                        {
                                ++src;
                                switch (*src)
                                {
                                        case 'n':
                                                id[x]='\n';
                                                break;
                                        case 'r':
                                                id[x]='\r';
                                                break;
                                        case 'a':
                                                id[x]=7;
                                                break;
                                        case 'b':
                                                id[x]=8;
                                                break;
                                        case '0':
                                                id[x]=0;
                                                break;
                                        case 'e':
                                                id[x]=27;
                                                break;
                                }
                                ++x;
                                ++src;
                        }
                        else
                        {
                                id[x++]=*(src++);
                        }
                }

                tok=TOK_STR;
                ++src;
                return;
        }
        else if (isalpha(*src))
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

STRING strings = {.next=NULL,.text=""};

int str_count=0;

STRING * new_string()
{
        STRING * x = malloc(sizeof(STRING));
        if (!x) exit(2);
        memcpy(x->text,id,sizeof(id));
        x->next = strings.next;
        strings.next = x;
        ++str_count;
        return x;
}

void clean_strings()
{
        STRING * x = strings.next;
        STRING * p = NULL;
        while (x)
        {
                if (p)
                {
                        free(p);
                }
                p = x;
                x = x->next;
        }
}

void clean_vars()
{
        Variable * x = list.next;
        Variable * prev = NULL;
        while (x != NULL)
        {
                if (prev)
                {
                        if (prev->name)
                        {
                                free(prev->name);
                                prev->name = NULL;
                        }
                        free(prev);
                        prev = NULL;
                }
                prev = x;
                x = x->next;
        }
        if (prev)
        {
                if (prev->name) {
                        free(prev->name);
                        prev->name = NULL;
                }
                free(prev);
                prev = NULL;
        }
        list.next=NULL;
        bpoff = 4;
}

void clean()
{
        clean_vars();
        clean_strings();
}

void synerr(const char *format, ...)
{
        va_list args;
        va_start(args, format);
        printf("hey, dumbass -> ");
        vprintf(format, args);
        printf("\n");
        va_end(args);
        exit(1);
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
        if (!*name)
        {
                return NULL;
        }
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

#ifdef ARCH_I386_JDECL
void create_arguments()
{
        int c=0;
        int start = 4;
        char * tmp = src;
        int tokt = tok;
        char tid[sizeof(id)];
        strcpy(tid,id);

        while (*src && tok && tok != ')')
        {
                if (tok == TOK_IDE)
                {
                        start += 4;
                }
                next();
        }

        src = tmp;
        tok = tokt;
        strcpy(id,tid);

        while (*src && tok && tok != ')')
        {
                if (tok == TOK_IDE)
                {
                        Variable * x = cvar(4, id, c);
                        x->assigned=1;
                        emit("\tmov eax,[ebp+%d]\n",start);
                        emit("\tmov [ebp-%d],eax\n",x->bpoff);
                        start-=4;
                        c = 0;
                }
                else if (tok == TOK_CONST)
                {
                        c = 1;
                }
                next();
        }
}
#endif

#ifdef ARCH_I386_CDECL
void create_arguments()
{
        int c=0;
        int start = 4;
        while (*src && tok && tok != ')')
        {
                if (tok == TOK_IDE)
                {
                        Variable * x = cvar(4, id, c);
                        x->assigned=1;
                        emit("\tmov eax,[ebp+%d]\n",bpoff);
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
#endif

int collect_arguments()
{
        int c=0;
        while (*src && tok && tok != ')')
        {
                use_eax=1;
                expr();
                ++c;
        }
        emit("\tpush eax\n");
        return c;
}

#define is_start() (use_eax)

Stack stck;

void body()
{
        static int level=1;
        int mylevel=level++;
        spush(&stck,mylevel);
        use_eax = 1;
        while (tok != '{') next();
        while (tok != '}')
        {
                expr();
        }
        spop(&stck);
        if (stop(&stck)!=mylevel)
        {
                tok=1;
        }
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

                case TOK_STR:
                {
                        STRING * x = new_string();
                        emit("\tmov e%cx,lit_%d\n",(use_eax)?'a':'b',str_count-1);
                } break;

                case TOK_INT:
                case TOK_SHORT:
                case TOK_CHAR:
                case TOK_VOID:
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
                                        case TOK_VOID:  synerr("can't have void variable"); break;
                                }
                                con=0;
                        }
                } break;

                case TOK_EXTERN:
                {
                        next();
                        emit("\textern %s\n",id);
                        skip_til(')');
                } break;

                case '<':
                {
                        expr();
                        emit("\tsub eax,ebx\n");
                        emit("\tcmp eax,0\n");
                        emit("\tsetl al\n");
                        emit("\tmovzx eax,al\n");
                } break;

                case '>':
                {
                        expr();
                        emit("\tsub eax,ebx\n");
                        emit("\tcmp eax,0\n");
                        emit("\tsetg al\n");
                        emit("\tmovzx eax,al\n");
                } break;

                case TOK_EQ:
                {
                        expr();
                        emit("\tsub eax,ebx\n");
                        emit("\tcmp eax,0\n");
                        emit("\tsete al\n");
                        emit("\tmovzx eax,al\n");
                } break;

                case TOK_NEQ:
                {
                        expr();
                        emit("\tsub eax,ebx\n");
                        emit("\tcmp eax,0\n");
                        emit("\tsetne al\n");
                        emit("\tmovzx eax,al\n");
                } break;

                case TOK_WHILE:
                {
                        int label = m++;
                        int end = m++;
                        emit(".M%d:\n",label);
                        expr();
                        emit("\ttest eax,eax\n");
                        emit("\tjz .M%d\n",end);
                        body();
                        emit("\tjmp .M%d\n",label);
                        emit(".M%d:\n",end);
                        return;
                } break;

                case TOK_BREAK:
                {
                        emit("\tjmp .M%d\n",m-2);
                } break;

                case TOK_IF:
                {
                        int label = m++;
                        int end = m++;
                        emit(".M%d:\n",label);
                        expr();
                        emit("\ttest eax,eax\n");
                        emit("\tjz .M%d\n",end);
                        body();
                        emit(".M%d:\n",end);
                } break;

                case TOK_IDE:
                {
                        PushID();
                        if (is_function_declaration())
                        {
                                clean();
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
                                emit(".exit:\n",nam);
                                emit("\tmov esp,ebp\n");
                                emit("\tpop ebp\n");
                                emit("\tret\n");
                        }
                        else if (is_function())
                        {
                                PopID();
                                char nam[64];
                                strcpy(nam,id);
                                int argc = collect_arguments();
                                emit("\tcall %s\n",nam);
                                emit("\tadd esp,%d\n",4*argc);
                        }
                        else if (!is_assignment())
                        {
                                PopID();
                                Variable * var = gvar(id);
                                if (!var)
                                {
                                        synerr("variable '%s' not found", id);
                                }
                                switch (var->size)
                                {
                                        case 4:emit("\tmov e%cx,[ebp-%d]\n",(use_eax)?'a':'b',var->bpoff); break;
                                        case 2:emit("\tmov %cx,[ebp-%d]\n",(use_eax)?'a':'b',var->bpoff); break;
                                        case 1:emit("\tmov %cl,[ebp-%d]\n",(use_eax)?'a':'b',var->bpoff); break;
                                }
                                use_eax=0;
                                expr();
                        }
                } break;

                case ',':
                {
                        emit("\tpush eax\n");
                        use_eax = 1;
                        return;
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
                                        synerr("syntax error, what did you even do? &*variable what the fuck");
                                }

                                PopID();
                                Variable * var = gvar(id);
                                if (!var)
                                {
                                        synerr("variable '%s' not found", id);
                                }
                                emit("\tlea eax,[ebp-%d] \n",var->bpoff);
                        }
                        else
                        {
                                expr();
                                emit("\tand eax,ebx\n");
                        }
                } break;

                case '*':
                {
                        if (is_start())
                        {
                                expr();
                                emit("\tmov eax,[eax]\n");
                        }
                        else
                        {
                                expr();
                                emit("\timul eax,ebx\n");
                        }
                } break;

                case '/':
                {
                        expr();
                        emit("\tcdq\n");
                        emit("\tidiv ebx\n");
                } break;

                case '%':
                {
                        expr();
                        emit("\tcdq\n");
                        emit("\tidiv ebx\n");
                        emit("\tmov eax,edx\n");
                } break;

                case '|':
                {
                        expr();
                        emit("\tor eax,ebx\n");
                } break;

                case '^':
                {
                        expr();
                        emit("\txor eax,ebx\n");
                } break;

                case '~':
                {
                        expr();
                        emit("\tnot eax\n");
                } break;

                case '!':
                {
                        expr();
                        emit("\txor eax,1\n");
                        emit("\tand eax,1\n");
                } break;

                case TOK_RETURN:
                {
                        expr();
                        emit("\tjmp .exit\n");
                        return;
                } break;

                case '=':
                {
                        PopID();
                        char name[64];
                        strcpy(name,id);
                        expr();

                        Variable * var = gvar(name);
                        if (!var)
                        {
                                synerr("variable '%s' not found", name);
                                return;
                        }

                        if (var->con && var->assigned)
                        {
                                synerr("can't assign to '%s', it's a constant", var->name);
                                return;
                        }

                        switch (var->size)
                        {
                                case 4:emit("\tmov [ebp-%d],eax\n",var->bpoff); break;
                                case 2:emit("\tmov [ebp-%d],ax\n",var->bpoff); break;
                                case 1:emit("\tmov [ebp-%d],al\n",var->bpoff); break;
                        }
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
#ifdef ARCH_I386_JDECL
        emit("\tglobal _start\n");
        emit("_start:\n");
        emit("\tcall main\n");
        emit("\tmov ebx,eax\n");
        emit("\tmov eax,1\n");
        emit("\tint 0x80\n");
#endif
        while (*src)
        {
                expr();
        }
        emit("\tsection .data\n");
        STRING * x = strings.next;
        int c=0;
        while (x)
        {
                emit("lit_%d: db ",c++);
                for (int i = 0; i < strlen(x->text); ++i)
                {
                        emit("%d, ", x->text[i]);
                }
                emit("0");
                x = x->next;
        }
        clean();
}
