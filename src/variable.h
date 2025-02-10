#ifndef VARIABLE_H
#define VARIABLE_H


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


VARIABLE * cvar(int size, char * name, int is_const)
{
        fprintf(fo,"\tsub esp,%d\n",size);
        VARIABLE * new = malloc(sizeof(VARIABLE));
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

#endif