extern malloc();
extern free();
extern printf();
extern putchar();
extern exit();

/*
   Statements don't need to be in functions,
   you just need to remember to exit from them
   lmao
*/
error:
        putchar(69);
        exit(1);

main()
{
        /* test */
        char * s = malloc(13);
        if (!s)
        {
                goto error;
                /* you could also call it with `error();` */
        }
        *s = 65;
        *(s+1) = 0;
        printf(s);
        free(s);
        return 0;
}