section .text
global main
extern putchar
extern getchar
main:
PUSH EBP
MOV EBP,ESP
SUB ESP,4
PUSH EAX
CALL getchar
ADD ESP,4*1
MOV [EBP-4],EAX
MOV EAX,[EBP-4]
PUSH EAX
CALL putchar
ADD ESP,4*1
MOV EAX,[EBP-4]
JMP .EXIT
.EXIT:
MOV ESP,EBP
POP EBP
RET
