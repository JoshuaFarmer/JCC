	section .text
	global main
	global _start
_start:
	call main
	mov ebx,eax
	mov eax,1
	int 0x80
main:
	push ebp
	mov ebp,esp
	mov eax,1
	test eax,eax
	jz L0
	mov eax,1
	test eax,eax
	jz L0
	mov eax,1
	jmp L1
L0:
	mov eax,0
L1:
	jmp .exit
.exit:
	mov esp,ebp
	pop ebp
	ret
	section .data
