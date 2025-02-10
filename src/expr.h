#include "types.h"
#include "id.h"
#include "tok.h"
#include "strings.h"
#include "variable.h"

void clean()
{
        clean_vars();
        clean_strings();
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
void declare_function_args()
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
                        VARIABLE * x = cvar(4, id, c);
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
void declare_function_args()
{
        int c=0;
        int start = 4;
        while (*src && tok && tok != ')')
        {
                if (tok == TOK_IDE)
                {
                        VARIABLE * x = cvar(4, id, c);
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

int call_function_args()
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

void HandleAssignment()
{
        PopID();
        char name[64];
        strcpy(name,id);
        expr();

        VARIABLE * var = gvar(name);
        if (!var)
        {
                synerr("VARIABLE '%s' not found", name);
                return;
        }

        if (!ConstIsAssignable(var))
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
}

void expr()
{
        static bool CONSTANT=false;
        static TYPE CURRENT_TYPE=TYPE_INT;
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
                        CONSTANT=true;
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
                        if (is_ptr) { cvar(4, id, CONSTANT); }
                        else
                        {
                                switch (type)
                                {
                                        case TOK_INT:   cvar(4, id, CONSTANT); break;
                                        case TOK_SHORT: cvar(2, id, CONSTANT); break;
                                        case TOK_CHAR:  cvar(1, id, CONSTANT); break;
                                        case TOK_VOID:  synerr("can't have void VARIABLE"); break;
                                }
                                CONSTANT=false;
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
                                declare_function_args();
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
                                int argc = call_function_args();
                                emit("\tcall %s\n",nam);
                                emit("\tadd esp,%d\n",4*argc);
                        }
                        else if (!is_assignment())
                        {
                                PopID();
                                VARIABLE * var = gvar(id);
                                if (!var)
                                {
                                        synerr("VARIABLE '%s' not found", id);
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
                                        synerr("syntax error, what did you even do? &*VARIABLE what the fuck");
                                }

                                PopID();
                                VARIABLE * var = gvar(id);
                                if (!var)
                                {
                                        synerr("VARIABLE '%s' not found", id);
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
                        HandleAssignment();
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
