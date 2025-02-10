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
	sub esp,4
	mov eax,1
	mov ebx,2
	sub eax,ebx
	cmp eax,0
	setl al
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
