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
	mov eax,2
	push eax
	mov eax,1
	push eax
	call foo
	add esp,4
	jmp .exit
.exit:
	mov esp,ebp
	pop ebp
	ret
foo:
	push ebp
	mov ebp,esp
	sub esp,4
	mov eax,[ebp+12]
	mov [ebp-4],eax
	sub esp,4
	mov eax,[ebp+8]
	mov [ebp-8],eax
	mov eax,[ebp-4]
	mov ebx,[ebp-8]
	sub eax,ebx
	jmp .exit
.exit:
	mov esp,ebp
	pop ebp
	ret
	section .data
