extern malloc();
extern free();
extern printf();
extern putchar();
extern exit();

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
        }
        *s = 65;
        *(s+1) = 0;
        printf(s);
        free(s);
        return 0;
}