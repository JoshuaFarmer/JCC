#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"expr.h"

void aerror()
{
	printf("One Or More Arguments Are Invalid.\n");
	exit(1);
}

FILE * fo;

int main(int argc, char ** argv)
{
	char * SourceFile = NULL;
	char * OutputFile = NULL;
	for (int i = 1; i < argc; ++i)
	{
		if (!strncmp(argv[i], "-o", 1000))
		{
			if (i == argc-1)
			{
				aerror();
			}
			OutputFile = argv[i+1];
			++i;
		}
		else
		{
			SourceFile = argv[i];
		}
	}

	if (!SourceFile) { aerror(); }
	if (!OutputFile)
	{
		OutputFile = "out";
	}

	FILE * sourceFp = fopen(SourceFile,"r");
	fo = fopen(OutputFile,"w");
	if (!sourceFp||!fo) { aerror(); }
	fseek(sourceFp, 0, SEEK_END);
	int length=ftell(sourceFp);
	fseek(sourceFp, 0, SEEK_SET);
	char * Buffer = malloc(length+12);
        if (!Buffer) return 1;
        memset(Buffer,0,length+12);
	int read = fread(Buffer, 1, length, sourceFp);
	if (read != length)
	{
		printf("The Fuck???");
		return(1);
	}

        fprintf(fo,"\tsection .text\n");
        fprintf(fo,"\tglobal main\n");
        compiler(Buffer);
	free(Buffer);
	fclose(sourceFp);
	return(0);
}
