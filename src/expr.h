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

#if defined(CALL_JDECL) && defined(ARCH_I386)
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

#elif defined(ARCH_CISC)
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
                        start += 2;
                }
                next();
        }

        src = tmp;
        tok = tokt;
        strcpy(id,tid);
        bpoff = -start-4;
        while (*src && tok && tok != ')')
        {
                if (tok == TOK_IDE)
                {
                        VARIABLE *x = cvar(TYPE_INT, id, c);
                        x->used=1;
                        c = 0;
                }
                else if (tok == TOK_CONST)
                {
                        c = 1;
                }
                next();
        }
        bpoff = 4;
}

#elif defined(ARCH_I8086)
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
                        start += 2;
                }
                next();
        }

        src = tmp;
        tok = tokt;
        strcpy(id,tid);
        bpoff = -start;
        while (*src && tok && tok != ')')
        {
                if (tok == TOK_IDE)
                {
                        VARIABLE * x = cvar(TYPE_INT, id, c);
                        x->used=1;
                        c = 0;
                }
                else if (tok == TOK_CONST)
                {
                        c = 1;
                }
                next();
        }
        bpoff = 2;
}

#elif defined(CALL_CDECL) && defined(ARCH_I386)
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
#elif defined(ARCH_I8085)
void declare_function_args()
{
        int c=0;
        while (*src && tok && tok != ')')
        {
                if (tok == TOK_IDE)
                {
                        VARIABLE * x = cvar(TYPE_INT, id, c);
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
#if defined(ARCH_I8085)
        emitting = false;
#endif
        while (*src && tok && tok != ')')
        {
                use_eax=1;
                expr();
                ++c;
        }
#if defined(ARCH_I386)
        emit("\tpush eax\n");
#elif defined(ARCH_I8086)
        emit("\tpush ax\n");
#elif defined(ARCH_CISC)
        emit("\tpush a\n");
#endif
        emitting = true;
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
                strcpy(current_function,nam);
                PopID();
                emit("%s:\n",nam);
#if defined(ARCH_I386)
                clean_vars();
                emit("\tpush ebp\n");
                emit("\tmov ebp,esp\n");
#elif defined(ARCH_I8086)||defined(ARCH_CISC)
                clean_vars();
                emit("\tpush bp\n");
                emit("\tmov bp,sp\n");
#elif defined(ARCH_I8085)
                emit("\t; function start\n");
#endif
                next();
                next();
                declare_function_args();
                body();
#if defined(ARCH_I386)
                emit("%s_exit:\n",current_function);
                emit("\tmov esp,ebp\n");
                emit("\tpop ebp\n");
                emit("\tret\n");
#elif defined(ARCH_I8086)||defined(ARCH_CISC)
                emit("%s_exit:\n",current_function);
                emit("\tmov sp,bp\n");
                emit("\tpop bp\n");
                emit("\tret\n");
#elif defined(ARCH_I8085)
                emit("%s_exit:\n",current_function);
                emit("\tret\n");
#endif
        }
        else if (is_function())
        {
                PopID();
                int argc = call_function_args();
#if defined(ARCH_I386)
                emit("\tcall %s\n",nam);
                emit("\tadd esp,%d\n",4*argc);
#elif defined(ARCH_CISC)
                emit("\tcall %s\n",nam);
                emit("\tadd sp,%d\n",4*argc);
#elif defined(ARCH_I8086)
                emit("\tcall %s\n",nam);
                emit("\tadd sp,%d\n",2*argc);
#elif defined(ARCH_I8085)
                emit("\tcall %s\n",nam);
#endif
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
#if defined(ARCH_I386)
                        emit("\tmov e%cx,%d\n",ActiveReg(),num);
#elif defined(ARCH_I8086)
                        emit("\tmov %cx,%d\n",ActiveReg(),num);
#elif defined(ARCH_CISC)
                        emit("\tmov %c,%d\n",ActiveReg(),num);
#elif defined(ARCH_I8085)
                        emit("\tmvi %c,%d\n",ActiveReg(),num);
#endif
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
                        STRING * x;
                        (!IN_ASM) ? x = new_string(),
#if defined(ARCH_I386)
                                    emit("\tmov e%cx,lit_%d\n",ActiveReg(),str_count-1)
#elif defined(ARCH_I8086)
                                    emit("\tmov %cx,lit_%d\n",ActiveReg(),str_count-1)
#elif defined(ARCH_CISC)
                                    emit("\tmov %c,lit_%d\n",ActiveReg(),str_count-1)
#elif defined(ARCH_I8085)
                                    emit("\tlxi %c,lit_%d\n",ActiveReg(),str_count-1)
#endif
                                  : emit("\t%s\n",id);
                        return;
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
                        int type=tok,is_ptr=get_var_name();
                        PopID();
                        switch (type)
                        {
                                case TOK_INT:   cvar((is_ptr) ? TYPE_INT_PTR   : TYPE_INT,   id, CONSTANT); break;
                                case TOK_SHORT: cvar((is_ptr) ? TYPE_SHORT_PTR : TYPE_SHORT, id, CONSTANT); break;
                                case TOK_CHAR:  cvar((is_ptr) ? TYPE_CHAR_PTR  : TYPE_CHAR,  id, CONSTANT); break;
                                case TOK_BCD:   cvar((is_ptr) ? TYPE_BCD_PTR   : TYPE_BCD,   id, CONSTANT); break;
                                case TOK_VOID:  (is_ptr) ? cvar(TYPE_VOID_PTR, id, CONSTANT) : synerr("can't have void variable"); break;
                        }
                        CONSTANT=false;
                } break;

                case TOK_EXTERN:
                {
                        next();
#if defined(ARCH_I386)
                        emit("\textern %s\n",id);
#else
                        synerr("Can't use extern for this architecture");
#endif
                        skip_til(')');
                } break;
                
                case TOK_CHAIN_OR:
                {
                        get_start_end();
                        get_args();

#if defined(ARCH_I386)
                        emit("\ttest eax,eax\n");
                        emit("\tjnz L%d\n", start);
                        use_eax = 1;
                        expr();
                        emit("\ttest eax,eax\n");
                        emit("\tjnz L%d\n", start);
                        emit("\tjmp L%d\n", end);
                        emit("L%d:\n", start);
                        emit("\tmov eax,1\n");
                        emit("L%d:\n", end);
#elif defined(ARCH_CISC)
                        emit("\ttest a,a\n");
                        emit("\tjnz L%d\n", start);
                        use_eax = 1;
                        expr();
                        emit("\ttest a,a\n");
                        emit("\tjnz L%d\n", start);
                        emit("\tjmp L%d\n", end);
                        emit("L%d:\n", start);
                        emit("\tmov a,1\n");
                        emit("L%d:\n", end);
#elif defined(ARCH_I8086)
                        emit("\ttest ax,ax\n");
                        emit("\tjnz L%d\n", start);
                        use_eax = 1;
                        expr();
                        emit("\ttest ax,ax\n");
                        emit("\tjnz L%d\n", start);
                        emit("\tjmp L%d\n", end);
                        emit("L%d:\n", start);
                        emit("\tmov ax,1\n");
                        emit("L%d:\n", end);
#elif defined(ARCH_I8085)
#endif
                } break;

                case TOK_CHAIN:
                {
                        get_start_end();
                        get_args();
#if defined(ARCH_I386)
                        emit("\ttest eax,eax\n");
                        emit("\tjz L%d\n", start);
                        use_eax = 1;
                        expr();
                        emit("\ttest eax,eax\n");
                        emit("\tjz L%d\n", start);
                        emit("\tmov eax,1\n");
                        emit("\tjmp L%d\n", end);
                        emit("L%d:\n", start);
                        emit("\tmov eax,0\n");
                        emit("L%d:\n", end);
#elif defined(ARCH_CISC)
                        emit("\ttest a,a\n");
                        emit("\tjz L%d\n", start);
                        use_eax = 1;
                        expr();
                        emit("\ttest a,a\n");
                        emit("\tjz L%d\n", start);
                        emit("\tmov a,1\n");
                        emit("\tjmp L%d\n", end);
                        emit("L%d:\n", start);
                        emit("\tmov a,0\n");
                        emit("L%d:\n", end);
#elif defined(ARCH_I8086)
                        emit("\ttest ax,ax\n");
                        emit("\tjz L%d\n", start);
                        use_eax = 1;
                        expr();
                        emit("\ttest ax,ax\n");
                        emit("\tjz L%d\n", start);
                        emit("\tmov ax,1\n");
                        emit("\tjmp L%d\n", end);
                        emit("L%d:\n", start);
                        emit("\tmov ax,0\n");
                        emit("L%d:\n", end);
#elif defined(ARCH_I8085)
#endif
                } break;

                case TOK_LEQ:
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        emit("\tsub eax,ebx\n");
                        emit("\tcmp eax,0\n");
                        emit("\tsetle al\n");
                        emit("\tmovzx eax,al\n");
#elif defined(ARCH_CISC)
                        get_start_end();
                        emit("\tsub a,b\n");
                        emit("\tjle %s_M%d\n",current_function,start);
                        emit("\tmov a,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmov a,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#elif defined(ARCH_I8086)
                        get_start_end();
                        emit("\tsub ax,bx\n");
                        emit("\tjle %s_M%d\n",current_function,start);
                        emit("\tmov ax,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmov ax,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#else
                        synerr("LEQ Is not supported by this architecture");
#endif
                } break;

                case TOK_GEQ:
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        emit("\tsub eax,ebx\n");
                        emit("\tcmp eax,0\n");
                        emit("\tsetge al\n");
                        emit("\tmovzx eax,al\n");
#elif defined(ARCH_CISC)
                        get_start_end();
                        emit("\tsub a,b\n");
                        emit("\tjge %s_M%d\n",current_function,start);
                        emit("\tmov a,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmov a,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#elif defined(ARCH_I8086)
                        get_start_end();
                        emit("\tsub ax,bx\n");
                        emit("\tjge %s_M%d\n",current_function,start);
                        emit("\tmov ax,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmov ax,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#else
                        synerr("GEQ Is not supported by this architecture");
#endif
                } break;

                case '<':
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        emit("\tsub eax,ebx\n");
                        emit("\tcmp eax,0\n");
                        emit("\tsetl al\n");
                        emit("\tmovzx eax,al\n");
#elif defined(ARCH_CISC)
                        get_start_end();
                        emit("\tsub a,b\n");
                        emit("\tjl %s_M%d\n",current_function,start);
                        emit("\tmov a,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmov a,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#elif defined(ARCH_I8086)
                        get_start_end();
                        emit("\tsub ax,bx\n");
                        emit("\tjl %s_M%d\n",current_function,start);
                        emit("\tmov ax,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmov ax,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#else
                        synerr("LE Is not supported by this architecture");
#endif
                } break;

                case '>':
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        emit("\tsub eax,ebx\n");
                        emit("\tcmp eax,0\n");
                        emit("\tsetg al\n");
                        emit("\tmovzx eax,al\n");
#elif defined(ARCH_CISC)
                        get_start_end();
                        emit("\tsub a,b\n");
                        emit("\tjg %s_M%d\n",current_function,start);
                        emit("\tmov a,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmov a,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#elif defined(ARCH_I8086)
                        get_start_end();
                        emit("\tsub ax,bx\n");
                        emit("\tjg %s_M%d\n",current_function,start);
                        emit("\tmov ax,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmov ax,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#else
                        synerr("GE Is not supported by this architecture");
#endif
                } break;

                case TOK_EQ:
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        emit("\tsub eax,ebx\n");
                        emit("\tcmp eax,0\n");
                        emit("\tsete al\n");
                        emit("\tmovzx eax,al\n");
#elif defined(ARCH_CISC)
                        get_start_end();
                        emit("\tsub a,b\n");
                        emit("\tjz %s_M%d\n",current_function,start);
                        emit("\tmov a,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmov a,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#elif defined(ARCH_I8085)
                        get_start_end();
                        emit("\tsub b\n");
                        emit("\tjz %s_M%d\n",current_function,start);
                        emit("\tmvi a,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmvi a,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#elif defined(ARCH_I8086)
                        get_start_end();
                        emit("\tsub ax,bx\n");
                        emit("\tje %s_M%d\n",current_function,start);
                        emit("\tmov ax,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmov ax,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#else
                        synerr("EQ Is not supported by this architecture");
#endif
                } break;

                case TOK_NEQ:
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        emit("\tsub eax,ebx\n");
                        emit("\tcmp eax,0\n");
                        emit("\tsetne al\n");
                        emit("\tmovzx eax,al\n");
#elif defined(ARCH_CISC)
                        get_start_end();
                        emit("\tsub a,b\n");
                        emit("\tjnz %s_M%d\n",current_function,start);
                        emit("\tmov a,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmov a,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#elif defined(ARCH_I8085)
                        get_start_end();
                        emit("\tsub b\n");
                        emit("\tjz %s_M%d\n",current_function,start);
                        emit("\tmvi a,1\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmvi a,0\n");
                        emit("\t%s_M%d:\n",current_function,end);
#elif defined(ARCH_I8086)
                        get_start_end();
                        emit("\tsub ax,bx\n");
                        emit("\tjne %s_M%d\n",current_function,start);
                        emit("\tmov ax,0\n");
                        emit("\tjmp %s_M%d\n",current_function,end);
                        emit("\t%s_M%d:\n",current_function,start);
                        emit("\tmov ax,1\n");
                        emit("\t%s_M%d:\n",current_function,end);
#else
                        synerr("NEQ Is not supported by this architecture");
#endif
                } break;

                case TOK_WHILE:
                {
                        get_start_end();
                        emit("%s_M%d:\n",current_function,start);
                        expr();
#if defined(ARCH_I386)
                        emit("\ttest eax,eax\n");
                        emit("\tjz %s_M%d\n",current_function,end);
#elif defined(ARCH_I8085)
                        emit("\tmov b,a\n");
                        emit("\tana b\n");
                        emit("\tjz %s_M%d\n",current_function,end);
#elif defined(ARCH_I8086)
                        emit("\ttest ax,ax\n");
                        emit("\tjz %s_M%d\n",current_function,end);
#elif defined(ARCH_CISC)
                        emit("\ttest a,a\n");
                        emit("\tjz %s_M%d\n",current_function,end);
#else
                        synerr("WHILE Is not supported by this architecture");
#endif
                        body();
                        emit("\tjmp %s_M%d\n",current_function,start);
                        emit("%s_M%d:\n",current_function,end);
                        return;
                } break;

                case TOK_IF:
                {
                        get_start_end();
                        emit("%s_M%d:\n",current_function,start);
                        expr();
#if defined(ARCH_I386)
                        emit("\ttest eax,eax\n");
                        emit("\tjz %s_M%d\n",current_function,end);
#elif defined(ARCH_I8085)
                        emit("\tmov b,a\n");
                        emit("\tana b\n");
                        emit("\tjz %s_M%d\n",current_function,end);
#elif defined(ARCH_I8086)
                        emit("\ttest ax,ax\n");
                        emit("\tjz %s_M%d\n",current_function,end);
#elif defined(ARCH_CISC)
                        emit("\ttest a,a\n");
                        emit("\tjz %s_M%d\n",current_function,end);
#else
                        synerr("IF Is not supported by this architecture");
#endif
                        body();
                        emit("%s_M%d:\n",current_function,end);
                } break;

                case TOK_IDE:
                {
                        HandleIdentifier();
                } break;

                case ',':
#if defined(ARCH_I386)
                        emit("\tpush eax\n");
#elif defined(ARCH_I8086)
                        emit("\tpush ax\n");
#elif defined(ARCH_CISC)
                        emit("\tpush a\n");
#endif
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
                                VARIABLE * var = gvar(id);
                                if (is_ptr)
                                        synerr("syntax error, what did you even do? &*VARIABLE what the fuck");
#if defined(ARCH_I386)
                                emit("\tlea e%cx,[ebp-%d]\n",ActiveReg(),var->bpoff);
#elif defined(ARCH_I8086)
                                emit("\tlea %cx,[ebp-%d]\n",ActiveReg(),var->bpoff);
#elif defined(ARCH_I386)
                                emit("\tmvi a,\n",(var->bpoff)-(0xFFFF - 0xFF));
#elif defined(ARCH_CISC)
                                emit("\tlea %c,[ebp-%d]\n",ActiveReg(),var->bpoff);
#else
                        synerr("ADDR OF Is not supported by this architecture");
#endif
                                PopID();
                                return;
                        }

                        expr();
#if defined(ARCH_I386)
                        emit("\tand eax,ebx\n");
#elif defined(ARCH_I8086)
                        emit("\tand ax,bx\n");
#elif defined(ARCH_CISC)
                        emit("\tand a,b\n");
#elif defined(ARCH_I8085)
                        emit("\tana b\n");
#endif
                } break;

                case '*':
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        (is_start()) ? emit("\tmov eax,[eax]\n")
                                     : emit("\timul eax,ebx\n");
#elif defined(ARCH_I8086)
                        (is_start()) ? emit("\tmov ax,[ax]\n")
                                     : emit("\timul ax,bx\n");
#elif defined(ARCH_CISC)
                        (is_start()) ? emit("\tmov a,[a]\n")
                                     : emit("\tmul a,b\n");
#elif defined(ARCH_I8085)
                        (is_start()) ? emit("\tmvi h,255\n"),
                                       emit("\tmov l,a\n"),
                                       emit("\tmov a,m\n")
                                     : synerr("Architecture does not support multiplication.");
#endif
                } break;

                case '/':
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        emit("\tcdq\n");
                        emit("\tidiv ebx\n");
#elif defined(ARCH_I8086)
                        emit("\tidiv bx\n");
#elif defined(ARCH_CISC)
                        emit("\tdiv a,b\n");
#else
                        synerr("Architecture does not support division.");
#endif
                } break;

                case '%':
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        emit("\tcdq\n");
                        emit("\tidiv ebx\n");
                        emit("\tmov eax,edx\n");
#elif defined(ARCH_CISC)
                        emit("\tdiv a,b\n");
                        emit("\tmov a,d\n");
#elif defined(ARCH_I8086)
                        emit("\tidiv bx\n");
                        emit("\tmov ax,dx\n");
#else
                        synerr("Architecture does not support division (modulo).");
#endif
                } break;

                case '|':
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        emit("\tor eax,ebx\n");
#elif defined(ARCH_I8086)
                        emit("\tor ax,bx\n");
#elif defined(ARCH_I8085)
                        emit("\tora b\n");
#elif defined(ARCH_CISC)
                        emit("\tor a,b\n");
#endif
                } break;

                case '^':
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        emit("\txor eax,ebx\n");
#elif defined(ARCH_I8086)
                        emit("\txor ax,bx\n");
#elif defined(ARCH_I8085)
                        emit("\txra b\n");
#elif defined(ARCH_CISC)
                        emit("\txor a,b\n");
#endif
                } break;

                case '~':
                {
                        expr();
#if defined(ARCH_I386)
                        emit("\tnot eax\n");
#elif defined(ARCH_I8086)
                        emit("\tnot ax\n");
#elif defined(ARCH_I8085)
                        emit("\tcma\n");
#elif defined(ARCH_CISC)
                        emit("\tnot a\n");
#endif
                } break;

                case '!':
                {
                        expr();
#if defined(ARCH_I386)
                        emit("\txor eax,1\n");
                        emit("\tand eax,1\n");
#elif defined(ARCH_I8086)
                        emit("\txor ax,1\n");
                        emit("\tand ax,1\n");
#elif defined(ARCH_I8085)
                        emit("\txri 1\n");
                        emit("\tani 1\n");
#elif defined(ARCH_CISC)
                        emit("\txorb a,1\n");
                        emit("\tandb a,1\n");
#endif
                } break;

                case TOK_RETURN:
                {
                        expr();
#if defined(ARCH_I386) || defined(ARCH_I8085) || defined(ARCH_I8086)
                        emit("\tjmp %s_exit\n",current_function);
#endif
                        return;
                } break;

                case '=':
                {
                        HandleAssignment();
                } break;

                case '+':
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        emit("\tadd eax,ebx\n");
#elif defined(ARCH_I8086)
                        emit("\tadd ax,bx\n");
#elif defined(ARCH_I8085)
                        emit("\tadd b\n");
#elif defined(ARCH_CISC)
                        emit("\tadd a,b\n");
#endif
                } break;

                case '-':
                {
                        get_args();
                        expr();
#if defined(ARCH_I386)
                        emit("\tsub eax,ebx\n");
#elif defined(ARCH_I8086)
                        emit("\tsub ax,bx\n");
#elif defined(ARCH_I8085)
                        emit("\tsub b\n");
#elif defined(ARCH_CISC)
                        emit("\tsub a,b\n");
#endif
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
