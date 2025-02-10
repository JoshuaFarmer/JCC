main()
{
        int x;
        asm ("mov dword[ebp-4],0x00");
        asm ("mov eax,dword[ebp-4]");
}