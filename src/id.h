#ifndef ID_H
#define ID_H

#include "types.h"

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
