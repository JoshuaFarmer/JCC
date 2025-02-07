extern printf(c);

main(const c, const v)
{
        int n;
        n = 0;
        while (n<10)
        {
                n=n+1;
        }
        return foo(2,1); /* it pushes in reverse order, and idk how to fix it */
}

foo(a, b)
{
        return a - b;
}
