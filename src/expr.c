#include "expr.h"

char *src;

int tok,prvtok=0;
char ident[32];

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

int typeofnext()
{
        char * x = src;
        char id[sizeof(ident)];
        while (*x == ' ' || *x == '\t' || *x == '\n') x++;
        if (*x == '/' && *(x + 1) == '*')
        {
                x += 2;
                while (*x && !(*x == '*' && *(x + 1) == '/'))
                {
                        x++;
                }
                if (*x)
                {
                        x += 2;
                }
        }

        if (isalpha(*x))
        {
                int i = 0;
                while (isalnum(*x) && i < 31) {
                        id[i++] = *x++;
                } id[i] = '\0';
                for (int j = 0; j < 31; ++j)
                {
                        if (strcmp(keys[j].text,id)==0)
                        {
                                return keys[j].tok;
                        }
                }
                return TOK_IDE;
        }
        return *x;
}

void next()
{
        prvtok = tok;
        while (*src == ' ' || *src == '\t' || *src == '\n') src++;
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
        tok = *src++;
}

int get_i()
{
        int i = 0;
        for (--src;isdigit(*(src));src++)
        {
                i *= 10;
                i += (*(src))-'0';
        }
        --src;
        next();
        return i;
}

int start=1;
VAR * list = NULL;

void cleanup()
{
        VAR * x = list;
        VAR * prev = NULL;
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
}

int bpoff=4;

void create_var(int size, char * name, int is_const)
{
        fprintf(fo,"SUB ESP,%d\n",size);
        if (!list)
        {
                list = malloc(sizeof(VAR));
                if (!list) exit(2);
                list->is_constant=is_const;
                list->name=strdup(name);
                list->size=size;
                list->next=NULL;
                list->assigned=0;
                list->bpoff=bpoff;
        }
        else
        {
                VAR * new = malloc(sizeof(VAR));
                if (!new) exit(2);
                new->is_constant=is_const;
                new->name=strdup(name);
                new->next=list->next;
                new->bpoff=bpoff;
                new->size=size;
                new->assigned=0;
                list->next=new;
        }
        bpoff += size;
}

VAR * get_var(const char * name)
{
        VAR * x = list;
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

void dumpvars()
{
        VAR * x = list;
        while (x != NULL)
        {
                printf("sizeof(%s)==%d,const=%d,bpoff=%d\n",x->name,x->size,x->is_constant,x->bpoff);
                x=x->next;
        }
}

int is_const=0;
int labels[1024];
int lapos=0;
int label=0;

void block()
{
        while (tok != '{' && tok)
                next();
        while (tok != '}' && tok)
                expr();
        next();
        start=1;
}

int calling = 0;

void expr()
{
        next();
        if (isdigit(tok))
        {
                int i = get_i();
                (start) ? fprintf(fo,"MOV EAX,%d\n", i) : fprintf(fo,"MOV EBX,%d\n", i);
                start = 0;
                return;
        }

        switch (tok)
        {
                case '{':
                {
                        block();
                } break;
                case TOK_RETURN:
                {
                        start=1;
                        while (tok != ';' && tok)
                                expr();
                        fprintf(fo,"JMP .EXIT\n");
                        break;
                }
                case TOK_IF:
                {
                        start=1;
                        fprintf(fo,"M%d:\n",label);
                        expr();
                        labels[lapos++]=label++;
                        fprintf(fo,"CMP EAX,0\n");
                        fprintf(fo,"JE M%d\n",label);
                        labels[lapos++]=label++;
                        start=1;
                        block();
                        fprintf(fo,"M%d:\n",labels[--lapos]);
                } break;
                case TOK_WHILE:
                {
                        start=1;
                        int org = label;
                        fprintf(fo,"M%d:\n",org);
                        expr();
                        labels[lapos++]=label++;
                        fprintf(fo,"CMP EAX,0\n");
                        fprintf(fo,"JE M%d\n",label);
                        labels[lapos++]=label++;
                        start=1;
                        block();
                        fprintf(fo,"JMP M%d\n",org);
                        fprintf(fo,"M%d:\n",labels[--lapos]);
                } break;
                case 0:
                case 13:
                case '}':
                case ')':
                case 10:
                break;
                case ',':
                case ';':
                {
                        start = 1;
                } break;
                case TOK_EXTERN:
                {
                        next();
                        fprintf(fo,"extern %s\n",ident);
                        while (tok != '(' && tok)
                                next();
                        while (tok != ')' && tok)
                                next();
                        next();
                } break;
                case TOK_CONST:
                {
                        is_const=1;
                } break;
                case TOK_CHAR:
                {
                        next();
                        if (tok == '*') {next(); create_var(4,ident,is_const);}
                        else create_var(1,ident,is_const);
                        is_const=0;
                } break;
                case TOK_INT:
                {
                        next();
                        if (tok == '*') {next(); create_var(4,ident,is_const);}
                        else create_var(4,ident,is_const);
                        is_const=0;
                } break;
                case TOK_SHORT:
                {
                        next();
                        if (tok == '*') {next(); create_var(4,ident,is_const);}
                        else create_var(2,ident,is_const);
                        is_const=0;
                } break;
                case '=':
                {
                        start=1;
                        char x[sizeof(ident)];
                        strcpy(x,ident);
                        while (tok != ';' && tok)
                                expr();
                        VAR * v = get_var(x);
                        if (v&&!(v->is_constant && v->assigned))
                        {
                                switch(v->size)
                                {
                                        case 4: fprintf(fo,"MOV [EBP-%d],EAX\n",v->bpoff); break;
                                        case 2: fprintf(fo,"MOV [EBP-%d],AX\n",v->bpoff); break;
                                        case 1: fprintf(fo,"MOV [EBP-%d],AL\n",v->bpoff); break;
                                }
                                v->assigned=1;
                        }
                        else if (v)
                        {
                                printf("CAN'T ASSIGN TO CONSTANT: %s\n",ident);
                                exit(1);
                        }
                        else
                        {
                                printf("VARIABLE NOT FOUND: %s\n",ident);
                                exit(1);
                        }
                } break;
                case TOK_IDE:
                {
                        char * tmp = src;
                        int t = tok;
                        next();
                        int newTok=tok;
                        src=tmp;
                        tok=t;
                        if (newTok=='(')
                        {
                                char name[sizeof(ident)];
                                strcpy(name,ident);

                                tmp = src;
                                while (tok != '(' && tok) next();
                                while (tok != ')' && tok) next();
                                next();
                                if (typeofnext() == '{')
                                {
                                        src=tmp;
                                        cleanup();
                                        fprintf(fo,"%s:\n",name);
                                        fprintf(fo,"PUSH EBP\n");
                                        fprintf(fo,"MOV EBP,ESP\n");
                                        block();
                                        fprintf(fo,".EXIT:\n");
                                        fprintf(fo,"MOV ESP,EBP\n");
                                        fprintf(fo,"POP EBP\n");
                                        fprintf(fo,"RET\n");
                                        return;
                                }

                                src=tmp;
                                calling=1;
                                while (tok != '(' && tok) next();
                                int x = 0;
                                while (tok != ')' && tok) {
                                        while (tok != ';' && tok && tok != ')' && tok != ',')
                                        {
                                                expr();
                                        }
                                        fprintf(fo,"PUSH EAX\n");
                                        ++x;
                                }
                                fprintf(fo,"CALL %s\n",name);
                                fprintf(fo,"ADD ESP,4*%d\n",x);
                                return;
                        }
                        if (newTok=='=') return;
                        VAR * v = get_var(ident);
                        if (v)
                        {
                                switch(v->size)
                                {
                                        case 4: fprintf(fo,"MOV E%cX,[EBP-%d]\n",(start)?'A':'B',v->bpoff); break;
                                        case 2: fprintf(fo,"MOV %cX,[EBP-%d]\n",(start)?'A':'B',v->bpoff); break;
                                        case 1: fprintf(fo,"MOV %cL,[EBP-%d]\n",(start)?'A':'B',v->bpoff); break;
                                }
                                start = 0;
                        }
                        else
                        {
                                printf("VARIABLE NOT FOUND: %s\n",ident);
                                exit(1);
                        }
                } break;
                case '(':
                {
                        while (tok != ')' && tok)
                        {
                                expr();
                        }
                        
                        next();
                        break;
                }
                case '+':
                case '-':
                {
                        int op = tok;
                        expr();
                        fprintf(fo,"%s EAX,EBX\n",(op=='+')?"ADD":"SUB");
                        break;
                }
                case '*':
                case '/':
                {
                        int op = tok;
                        if (prvtok==TOK_IDE||isdigit(prvtok))
                        {
                                expr();
                                fprintf(fo,"%s EAX,EBX\n",(op=='*')?"IMUL":"IDIV");
                        }
                        else
                        {
                                char * tmp = src;
                                int t = tok;
                                next();
                                char id[sizeof(ident)];
                                strcpy(id,ident);
                                next();
                                int newTok=tok;
                                src=tmp;
                                tok=t;
                                if (newTok=='=')
                                {
                                        tmp = src;
                                        src = id;
                                        expr();
                                        src = tmp;
                                        expr();
                                        next();
                                        start=1;
                                        fprintf(fo,"MOV EDI,EAX\n");
                                        expr();
                                        fprintf(fo,"MOV [EDI],EAX\n");
                                        return;
                                }
                                expr();
                                fprintf(fo,"MOV EAX,[EAX]\n");
                                start = 0;
                                next();
                        }
                        break;
                }
                case '!':
                case '~':
                {
                        expr();
                        fprintf(fo,"NOT EAX\n");
                        break; 
                }
                case '|':
                case '&':
                {
                        int op = tok;
                        if ((prvtok==TOK_IDE) || isdigit(prvtok))
                        {
                                expr();
                                fprintf(fo,"%s EAX,EBX\n",(op=='|')?"OR":"AND");
                        }
                        else
                        {
                                char * tmp = src;
                                int t = tok;
                                next();
                                int newTok=tok;
                                src=tmp;
                                tok=t;
                                if (newTok=='=') return; // invalid
                                VAR * v = get_var(ident);
                                if (v)
                                {
                                        fprintf(fo,"LEA EAX,[EBP-%d]\n",v->bpoff);
                                        start = 0;
                                }
                                else
                                {
                                        printf("VARIABLE NOT FOUND: %s\n",ident);
                                        exit(1);
                                }
                                next();
                        }
                        break;
                }
                case '^':
                {
                        expr();
                        fprintf(fo,"XOR EAX,EBX\n");
                        break;
                }

                default:
                {
                        printf("Unsupported Token: %d\n", tok);
                }
        }
}
