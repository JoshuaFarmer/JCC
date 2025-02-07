section .text
global main
extern malloc
extern free
extern printf
extern putchar
extern exit
<<<<<<< HEAD
main:
PUSH EBP
MOV EBP,ESP
SUB ESP,4
MOV EAX,DWORD[EBP+8]
MOV DWORD[EBP-4],EAX
SUB ESP,4
MOV EAX,DWORD[EBP+12]
MOV DWORD[EBP-8],EAX
MOV EAX,[EBP-8]
MOV EAX,[EAX]
PUSH EAX
CALL printf
ADD ESP,4*1
.EXIT:
MOV ESP,EBP
POP EBP
RET
=======
error:
MOV EAX,69
PUSH EAX
CALL putchar
ADD ESP,4*1
MOV EAX,1
PUSH EAX
CALL exit
ADD ESP,4*1
>>>>>>> master
