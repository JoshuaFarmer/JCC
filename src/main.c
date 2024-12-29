#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"tok.h"

void aerror()
{
	printf("One Or More Arguments Are Invalid.\n");
	exit(1);
}

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
	if (sourceFp == NULL) { aerror(); }

	/* get file length */
	fseek(sourceFp, 0, SEEK_END);
	int length=ftell(sourceFp);
	rewind(sourceFp);

	char * Buffer = malloc(length);
	int read = fread(Buffer, length, 1, sourceFp);
	if (read != length)
	{
		printf("The Fuck???");
		return(1);
	}

	printf("%s\n",Buffer);

	free(Buffer);
	fclose(sourceFp);
	return(0);
}
