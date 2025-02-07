	section .text
	global main
	extern printf
main:
	push ebp
	mov ebp,esp
	sub esp,4
	mov eax,[ebp+8]
	mov [ebp-4],eax
	sub esp,4
	mov eax,[ebp+12]
	mov [ebp-8],eax
	sub esp,4
	mov eax,0
	mov [ebp-12],eax 
.M0:
	mov eax,[ebp-12]
	mov ebx,10
	sub eax,ebx
	cmp eax,0
	setl al
	movzx eax,al
	test eax,eax
	jz .M1
	mov eax,[ebp-12]
	mov ebx,1
	add eax,ebx
	mov [ebp-12],eax 
	jmp .M0
.M1:
	mov eax,[ebp-12]
	mov ebx,7
	or eax,ebx
main.exit:
	mov esp,ebp
	pop ebp
	ret
