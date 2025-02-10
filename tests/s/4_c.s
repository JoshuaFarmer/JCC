	section .text
	global main
main:
	push ebp
	mov ebp,esp
	mov eax,0
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
