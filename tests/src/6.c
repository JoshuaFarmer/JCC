main()
{
        int x;
        asm ("mov dword[ebp-4],0x40");
        asm ("mov eax,dword[ebp-4]");
        return x;
}