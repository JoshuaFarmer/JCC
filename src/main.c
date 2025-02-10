#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"compile.h"

int main(int argc, char ** argv)
{
	char * s = NULL;
	char * o = NULL;
	for (int i = 1; i < argc; ++i)
	{
		if (!strncmp(argv[i], "-o", 1000))
		{
			if (i == argc-1)
			{
				aerror();
			}
			o = argv[i+1];
			++i;
		}
		else
		{
			s = argv[i];
		}
	}

	if (!s || !o)
        {
                aerror();
        }

        compiler(s,o);
	return(0);
}
