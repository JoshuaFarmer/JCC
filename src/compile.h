#ifndef COMPILE_H
#define COMPILE_H

char * mainPath = NULL;
char * outPath = NULL;
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
        mainPath = srcp;
        outPath = outp;

	int len=flen(sf);
	src = malloc(len+1);
        if (!src)
        {
                aerror();
                return;
        }

        memset(src,0,len+1);
	fread(src,1,len,sf);
        char* bf=src;

        if (!is_included)
        {
#if defined(ARCH_I386)
        emit("\tsection .text\n");
        emit("\tglobal main\n");
        #if defined(CALL_JDECL)
                emit("\tglobal _start\n");
                emit("_start:\n");
                emit("\tcall main\n");
                emit("\tmov ebx,eax\n");
                emit("\tmov eax,1\n");
                emit("\tint 0x80\n");
        #endif
#elif defined(ARCH_I8086)
        emit("\tbits 16\n");
        emit("\tglobal _start\n");
        emit("\tsection .text\n");
        #if defined(OS_DOS)
                emit("\torg 0x100\n");
        #endif
        emit("_start:\n");
        emit("\tcall main\n");
        #if defined(OS_DOS)
                emit("\tmov ax,0x4c00\n");
                emit("\tint 0x21\n");
        #endif
        emit("\tjmp $\n");
#elif defined(ARCH_I8085)
        /* https://www.sim8085.com/ */
        emit("start:\n");
        emit("\tlxi h,0xff00\n");
        emit("\tsphl\n");
        emit("\tcall main\n");
        emit("inf:\n");
        emit("\thlt\n");
        emit("\tjmp inf\n");
        bpoff = 0xFFFF;
#elif defined(ARCH_CISC)
        emit("_start:\n");
        emit("\tmov sp,8192\n");
        emit("\tcall main\n");
        emit("\thlt\n");
#endif
        }
        while (*src)
        {
                expr();
        }
#if !defined(ARCH_I8085) && !defined(ARCH_CISC)
        emit("\tsection .data\n");
#endif
        STRING * x = strings.next;
        int c=0;
        while (x)
        {
                emit("lit_%d:\ndb ",c++);
                for (int i = 0; i < strlen(x->text); ++i)
                {
                        emit("%d, ", x->text[i]);
                }
                emit("0");
                x = x->next;
        }

#if !defined(ARCH_I8085) && !defined(ARCH_CISC)
        if (is_included)
                emit("\tsection .text\n");
#elif defined(ARCH_CISC)
        emit("\t; end of file");
#endif
        clean();
	free(bf);
	fclose(sf);
	fclose(fo);
}

#endif
