fib(n)
{
        int a;
        int b;
        int c;
        a=c=0;
        b=1;
        while (n > 0)
        {
                a = b;
                b = c;
                c = a + b;
                n = n - 1;
        } 
        return c;
}

main()
{
        return fib(6);
}
