	section .text
	global main
	extern putchar
main:
	push ebp
	mov ebp,esp
	sub esp,4
	mov eax,[ebp+4]
	mov [ebp-4],eax
	sub esp,4
	mov eax,[ebp+8]
	mov [ebp-8],eax
	mov eax,65
	push eax
	call putchar
main.exit:
	mov esp,ebp
	pop ebp
	ret
