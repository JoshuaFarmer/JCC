#ifndef GLOBALS_H
#define GLOBALS_H

void compiler(char *,char *);
void expr();

FILE * fo;
FILE * sf;
VARIABLE list;
char   * src,id[64];
int      tok,num,bpoff=4;
int      use_eax=1,m = 0;
uint8_t  id_stack[256][64];
uint8_t  id_stack_ptr = 0;
int      str_count=0;
STRING   strings = {.next=NULL,.text=""};
STACK    stck;

bool ConstIsAssignable(VARIABLE * var)
{
        return var->con ? (var->con && !var->assigned) : true;
}

void emit(const char *format, ...)
{
        va_list args;
        va_start(args, format);
        vfprintf(fo, format, args);
        va_end(args);
}

int flen(FILE * fp)
{
        fseek(fp, 0, SEEK_END);
	int length=ftell(sf);
	fseek(fp, 0, SEEK_SET);
        return length;
}

void aerror()
{
        printf("One Or More Arguments Are Invalid.\n");
        exit(1);
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

#endif
