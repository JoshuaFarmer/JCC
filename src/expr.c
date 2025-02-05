#include "expr.h"

char *src;

int tok;
char ident[32];

struct KEY keys[]=
{
        {.text="auto",TOK_AUTO},
        {.text="break",TOK_BREAK},
        {.text="case",TOK_CASE},
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
        if (isalpha(*x))
        {
                int i = 0;
                while (isalnum(*x) && i < 31) {
                        id[i++] = *x++;
                } id[i] = '\0';
                for (int j = 0; j < 30; ++j)
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
        while (*src == ' ' || *src == '\t' || *src == '\n') src++;
        if (isalpha(*src))
        {
                int i = 0;
                while (isalnum(*src) && i < 31) {
                        ident[i++] = *src++;
                } ident[i] = '\0';
                for (int j = 0; j < 30; ++j)
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
        while (isdigit(tok))
        {
                i *= 10;
                i += tok-'0';
                tok=*src++;
        }
        --src;
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
        printf("SUB ESP,%d\n",size);
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
        while (tok != '{')
                next();
        while (tok != '}')
                expr();
        start=1;
}

void expr()
{
        next();
        if (isdigit(tok))
        {
                int i = get_i();
                (start) ? printf("MOV EAX,%d\n", i) : printf("MOV EBX,%d\n", i);
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
                        printf("RET\n");
                        break;
                }
                case TOK_IF:
                {
                        start=1;
                        printf("M%d:\n",label);
                        expr();
                        labels[lapos++]=label++;
                        printf("CMP EAX,0\n");
                        printf("JE M%d\n",label);
                        labels[lapos++]=label++;
                        start=1;
                        block();
                        printf("M%d:\n",labels[--lapos]);
                } break;
                case TOK_WHILE:
                {
                        start=1;
                        int org = label;
                        printf("M%d:\n",org);
                        expr();
                        labels[lapos++]=label++;
                        printf("CMP EAX,0\n");
                        printf("JE M%d\n",label);
                        labels[lapos++]=label++;
                        start=1;
                        block();
                        printf("JMP M%d\n",org);
                        printf("M%d:\n",labels[--lapos]);
                } break;
                case 0:
                case 13:
                case '}':
                case ')':
                case 10:
                break;
                case ';':
                {
                        start = 1;
                } break;
                case TOK_CONST:
                {
                        is_const=1;
                } break;
                case TOK_INT:
                {
                        next();
                        create_var(4,ident,is_const);
                        is_const=0;
                } break;
                case TOK_SHORT:
                {
                        next();
                        create_var(2,ident,is_const);
                        is_const=0;
                } break;
                case TOK_CHAR:
                {
                        next();
                        create_var(1,ident,is_const);
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
                                        case 4: printf("MOV [EBP-%d],EAX\n",v->bpoff); break;
                                        case 2: printf("MOV [EBP-%d],AX\n",v->bpoff); break;
                                        case 1: printf("MOV [EBP-%d],AL\n",v->bpoff); break;
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
                        if (newTok=='=') return;
                        VAR * v = get_var(ident);
                        if (v)
                        {
                                switch(v->size)
                                {
                                        case 4: printf("MOV E%cX,[EBP-%d]\n",(start)?'A':'B',v->bpoff); break;
                                        case 2: printf("MOV %cX,[EBP-%d]\n",(start)?'A':'B',v->bpoff); break;
                                        case 1: printf("MOV %cL,[EBP-%d]\n",(start)?'A':'B',v->bpoff); break;
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
                        expr();
                        if (tok != ')')
                                next();
                        if (tok != ')')
                        {
                                printf("Error: Expected ')'\n");
                                exit(1);
                        }
                        
                        next();
                        break;
                }
                case '+':
                case '-':
                {
                        int op = tok;
                        expr();
                        printf("%s EAX,EBX\n",(op=='+')?"ADD":"SUB");
                        break;
                }
                case '*':
                case '/':
                {
                        int op = tok;
                        expr();
                        printf("%s EAX,EBX\n",(op=='*')?"MUL":"DIV");
                        break;
                }
                case '!':
                case '~':
                {
                        expr();
                        printf("NOT EAX\n");
                        break; 
                }
                case '|':
                case '&':
                {
                        int op = tok;
                        expr();
                        printf("%s EAX,EBX\n",(op=='|')?"OR":"AND");
                        break;
                }
                case '^':
                {
                        expr();
                        printf("XOR EAX,EBX\n");
                        break;
                }

                default:
                {
                        printf("Unsupported Token: %d\n", tok);
                }
        }
}
