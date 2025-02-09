extern printf(c);

main(const c, const v)
{
        void * p;
        p = v+4; /*argv[1] (arrays dont exist yet)*/
        printf(*p);
        int n;
        n = 0;
        while (n<10)
        {
                n=n+1;
        }
        /*
           it pushes arguments in reverse order,
           and idk how to fix it (So this returns 255
           insteaed of 1)
        */
        return foo(2,1);
}

foo(a, b)
{
        return a - b;
}
