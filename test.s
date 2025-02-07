section .text
global main
extern malloc
extern free
extern printf
extern putchar
extern exit
error:
MOV EAX,69
PUSH EAX
CALL putchar
ADD ESP,4*1
MOV EAX,1
PUSH EAX
CALL exit
ADD ESP,4*1
