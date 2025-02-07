extern printf(c);

/*
        we dont have return types
        or argument types...
        (but u can make them const)
*/

main(const c, const v)
{
        int n;
        n = 0;
        while (n<10)
        {
                n=n+1;
        }
        return foo(2,1);
}

foo(a, b)
{
        return a - b;
}
