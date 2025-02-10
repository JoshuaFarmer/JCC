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
                        VARIABLE * x = cvar(TYPE_INT, id, c);
                        x->used=1;
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
                        VARIABLE * x = cvar(TYPE_INT, id, c);
                        x->used=1;
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
        SaveIdAs(name);
        expr();
        VARIABLE * var = gvar(name);
        mov_variable_exx(var);
}

void HandleIdentifier()
{
        PushID();
        SaveIdAs(nam);
        if (is_function_declaration())
        {
                clean_vars();
                PopID();
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
                int argc = call_function_args();
                emit("\tcall %s\n",nam);
                emit("\tadd esp,%d\n",4*argc);
        }
        else if (!is_assignment())
        {
                PopID();
                VARIABLE * var = gvar(id);
                mov_exx_variable(var);
                push_type(var->type);
                expr();
        }
}

void expr()
{
        static bool CONSTANT=false;
        static bool IN_ASM=false;
        static TYPE CURRENT_TYPE=TYPE_INT;
        next();
        switch(tok)
        {
                case TOK_NUM:
                {
                        emit("\tmov e%cx,%d\n",(use_eax)?'a':'b',num);
                        push_type(TYPE_INT);
                        use_eax=0;
                        expr();
                } break;

                case TOK_CONST:
                {
                        CONSTANT=true;
                } break;

                case TOK_STR:
                {
                        if (!IN_ASM)
                        {
                                STRING * x = new_string();
                                emit("\tmov e%cx,lit_%d\n",(use_eax)?'a':'b',str_count-1);
                        }
                        else
                        {
                                emit("\t%s\n",id);
                                return;
                        }
                } break;

                case TOK_ASM:
                {
                        IN_ASM = true;
                        skip_til('(');
                        expr();
                        skip_til(')');
                        IN_ASM = false;
                } break;

                case TOK_INT:
                case TOK_SHORT:
                case TOK_CHAR:
                case TOK_BCD:
                case TOK_VOID:
                {
                        int type = tok;
                        bool is_ptr = get_var_name();
                        PopID();
                        if (is_ptr)
                        {
                                switch (type)
                                {
                                        case TOK_INT: cvar(TYPE_INT_PTR, id, CONSTANT);
                                        case TOK_SHORT: cvar(TYPE_INT_PTR, id, CONSTANT);
                                        case TOK_CHAR: cvar(TYPE_CHAR_PTR, id, CONSTANT);
                                }
                        }
                        else
                        {
                                switch (type)
                                {
                                        case TOK_INT:   cvar(TYPE_INT, id, CONSTANT); break;
                                        case TOK_SHORT: cvar(TYPE_SHORT, id, CONSTANT); break;
                                        case TOK_CHAR:  cvar(TYPE_CHAR, id, CONSTANT); break;
                                        case TOK_BCD:  cvar(TYPE_BCD, id, CONSTANT); break;
                                        case TOK_VOID:  synerr("can't have void variable"); break;
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
                
                case TOK_CHAIN_OR:
                {
                        int true_label = m++;
                        int end = m++;
                        int type_b = pop_type();
                        int type_a = pop_type();
                        typeCheck(type_a, type_b);

                        emit("\ttest eax,eax\n");
                        emit("\tjnz L%d\n", true_label);
                        use_eax = 1;
                        expr();
                        emit("\ttest eax,eax\n");
                        emit("\tjnz L%d\n", true_label);
                        emit("\tjmp L%d\n", end);
                        emit("L%d:\n", true_label);
                        emit("\tmov eax,1\n");
                        emit("L%d:\n", end);
                } break;

                case TOK_CHAIN:
                {
                        int false_label = m++;
                        int end = m++;
                        int type_b = pop_type();
                        int type_a = pop_type();
                        typeCheck(type_a,type_b);

                        emit("\ttest eax,eax\n");
                        emit("\tjz L%d\n", false_label);
                        use_eax = 1;
                        expr();
                        emit("\ttest eax,eax\n");
                        emit("\tjz L%d\n", false_label);
                        emit("\tmov eax,1\n");
                        emit("\tjmp L%d\n", end);
                        emit("L%d:\n", false_label);
                        emit("\tmov eax,0\n");
                        emit("L%d:\n", end);
                } break;

                case TOK_LEQ:
                {
                        int type_b = pop_type();
                        int type_a = pop_type();
                        typeCheck(type_a,type_b);
                        expr();
                        emit("\tsub eax,ebx\n");
                        emit("\tcmp eax,0\n");
                        emit("\tsetle al\n");
                        emit("\tmovzx eax,al\n");
                } break;

                case TOK_GEQ:
                {
                        expr();
                        emit("\tsub eax,ebx\n");
                        emit("\tcmp eax,0\n");
                        emit("\tsetge al\n");
                        emit("\tmovzx eax,al\n");
                } break;

                case '<':
                {
                        int type_b = pop_type();
                        int type_a = pop_type();
                        typeCheck(type_a,type_b);
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
                        HandleIdentifier();
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
