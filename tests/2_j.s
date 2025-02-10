	section .text
	global main
	global _start
_start:
	call main
	mov ebx,eax
	mov eax,1
	int 0x80
fib:
	push ebp
	mov ebp,esp
	sub esp,4
	mov eax,[ebp+8]
	mov [ebp-4],eax
	sub esp,4
	sub esp,4
	sub esp,4
	mov [ebp-8],eax
	mov eax,0
	mov [ebp-16],eax
	mov eax,1
	mov [ebp-12],eax
.M0:
	mov eax,[ebp-4]
	mov ebx,0
	sub eax,ebx
	cmp eax,0
	setg al
	movzx eax,al
	test eax,eax
	jz .M1
	mov eax,[ebp-12]
	mov [ebp-8],eax
	mov eax,[ebp-16]
	mov [ebp-12],eax
	mov eax,[ebp-8]
	mov ebx,[ebp-12]
	add eax,ebx
	mov [ebp-16],eax
	mov eax,[ebp-4]
	mov ebx,1
	sub eax,ebx
	mov [ebp-4],eax
	jmp .M0
.M1:
	mov eax,[ebp-16]
	jmp .exit
.exit:
	mov esp,ebp
	pop ebp
	ret
main:
	push ebp
	mov ebp,esp
	mov eax,6
	push eax
	call fib
	add esp,4
	jmp .exit
.exit:
	mov esp,ebp
	pop ebp
	ret
	section .data
