#ifndef VARIABLE_H
#define VARIABLE_H

#include <assert.h>

void clean_vars()
{
        VARIABLE * x = list.next;
        VARIABLE * prev = NULL;
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

VARIABLE * cvar(TYPE type, char * name, int is_const)
{
        int size = SIZEOF(type);
        //fprintf(fo,"\tsub esp,%d\n",size);
        VARIABLE * new = malloc(sizeof(VARIABLE));
        if (!new) exit(2);
        new->con=is_const;
        new->name=strdup(name);
        new->next=list.next;
        new->bpoff=bpoff;
        new->size=size;
        new->used=0;
        new->type=type;
        list.next=new;
        bpoff += size;
        return new;
}

VARIABLE * gvar(const char * name)
{
        if (!*name)
        {
                return NULL;
        }
        VARIABLE * x = list.next;
        while (x != NULL)
        {
                if (strncmp(name,x->name,32)==0)
                {
                        return x;
                }
                x=x->next;
        }

        synerr("VARIABLE '%s' not found", name);
        return NULL;
}

void vars()
{
        VARIABLE * x = list.next;
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

void mov_exx_variable(VARIABLE * var)
{
        switch (var->size)
        {
#if defined(ARCH_I386)
                case 4:emit("\tmov e%cx,[ebp-%d]\n",(use_eax)?'a':'b',var->bpoff); break;
                case 2:emit("\tmov %cx,[ebp-%d]\n",(use_eax)?'a':'b',var->bpoff); break;
                case 1:emit("\tmov %cl,[ebp-%d]\n",(use_eax)?'a':'b',var->bpoff); break;
#elif defined(ARCH_I8085)
                case 4:
                case 2:
                case 1:emit("\tlxi d,%d ; %d\n",(unsigned char)(char)(-var->bpoff),(char)(-var->bpoff));
                       emit("\tdad d\n");
                       emit("\tmov %c,m\n",(use_eax)?'a':'b');
                       emit("\tlxi d,%d\n",(var->bpoff));
                       emit("\tdad d\n");
#endif
        }
        use_eax=0;
}

void mov_variable_exx(VARIABLE * var)
{
        if (!ConstIsAssignable(var))
        {
                synerr("can't assign to '%s', it's a constant", var->name);
                return;
        }

        switch (var->size)
        {
#if defined(ARCH_I386)
                case 4:emit("\tmov [ebp-%d],eax\n",var->bpoff); break;
                case 2:emit("\tmov [ebp-%d],ax\n",var->bpoff); break;
                case 1:emit("\tmov [ebp-%d],al\n",var->bpoff); break;
#elif defined(ARCH_I8085)
                case 4:
                case 2:
                case 1:emit("\tlxi d,%d; %d\n",(unsigned char)(char)(-var->bpoff),(char)(-var->bpoff));
                       emit("\tdad d\n");
                       emit("\tmov m,%c\n",(use_eax)?'a':'b');
                       emit("\tlxi d,%d\n",(var->bpoff));
                       emit("\tdad d\n");
#endif
        }
        var->used = true;
}

TYPE    type_stack[256];
uint8_t type_stackptr=0;

void push_type(TYPE t)
{
        type_stack[type_stackptr++] = t;
}

TYPE pop_type()
{
        return type_stack[--type_stackptr];
}

void typeCheck(int a, int b)
{
        if (a == TYPE_BCD && b == TYPE_BCD) return;
        else if (a == TYPE_BCD || b == TYPE_BCD)
        {
                synerr("Invalid BCD Type Usage");
        }
}

#endif