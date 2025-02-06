extern malloc();
extern free();
extern printf();

main()
{
        char * s = malloc(13);
        *s = 65;
        *(s+1) = 0;
        printf(s);
        free(s);
        return 0;
}