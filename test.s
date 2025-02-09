	section .text
	global main
	extern printf
main:
	push ebp
	mov ebp,esp
	mov eax,lit_0
	push eax
	call printf
	add esp,4
	mov eax,0
	jmp .exit
.exit:
	mov esp,ebp
	pop ebp
	ret
	section .data
lit_0: db 72, 101, 108, 108, 111, 44, 32, 87, 111, 114, 108, 100, 33, 10, 0