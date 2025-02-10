#ifndef STRINGS_H
#define STRINGS_H

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

#endif
