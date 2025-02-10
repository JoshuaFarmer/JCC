	section .text
	global main
main:
	push ebp
	mov ebp,esp
	sub esp,4
	mov eax,3
	mov ebx,2
	sub eax,ebx
	cmp eax,0
	setge al
	movzx eax,al
	mov [ebp-4],eax
	mov eax,[ebp-4]
	mov ebx,2
	or eax,ebx
	jmp .exit
.exit:
	mov esp,ebp
	pop ebp
	ret
	section .data
