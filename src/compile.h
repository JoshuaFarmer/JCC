#ifndef COMPILE_H
#define COMPILE_H

#include "types.h"
#include "expr.h"

void compiler(char * srcp, char * outp)
{
        sf = fopen(srcp,"r");
	fo = fopen(outp,"w");
	if (!sf||!fo)
        {
                aerror();
        }

	int len=flen(sf);
	src = malloc(len+1);
        if (!src)
        {
                return;
        }

        memset(src,0,len+1);
	fread(src,1,len,sf);
        char* bf=src;

        emit("\tsection .text\n");
        emit("\tglobal main\n");
#ifdef ARCH_I386_JDECL
        emit("\tglobal _start\n");
        emit("_start:\n");
        emit("\tcall main\n");
        emit("\tmov ebx,eax\n");
        emit("\tmov eax,1\n");
        emit("\tint 0x80\n");
#endif
        while (*src)
        {
                expr();
        }
        emit("\tsection .data\n");
        STRING * x = strings.next;
        int c=0;
        while (x)
        {
                emit("lit_%d: db ",c++);
                for (int i = 0; i < strlen(x->text); ++i)
                {
                        emit("%d, ", x->text[i]);
                }
                emit("0");
                x = x->next;
        }

        clean();
	free(bf);
	fclose(sf);
	fclose(fo);
}

#endif
