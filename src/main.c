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

	if (!SourceFile)
	{
		printf("No Source File Provided\n");
	}

	if (!OutputFile)
	{
		OutputFile = "out";
	}

	FILE * sourceFp = fopen(SourceFile,"r");
	if (sourceFp == NULL)
	{
		return (1);
	}

	do
	{
		char c = fgetc(sourceFp);
		if (feof(sourceFp))
		{
			break;
		}

		printf("%c",c);
	}
	while (1);

	fclose(sourceFp);
	return(0);
}
