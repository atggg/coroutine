.data
.const
.code
jmps proc
	mov [rcx],rax
	mov [rcx+8h],rbx
	mov [rcx+10h],rcx
	mov [rcx+18h],rdx
	mov [rcx+20h],rsi
	mov [rcx+28h],rdi
	mov [rcx+30h],r8
	mov [rcx+38h],r9
	mov [rcx+40h],r10
	mov [rcx+48h],r11
	mov [rcx+50h],r12
	mov [rcx+58h],r13
	mov [rcx+60h],r14
	mov [rcx+68h],r15
	add rsp,8h
	mov [rcx+70h],rsp
	sub rsp,8h
	mov rax,[rsp]
	mov [rcx+78h],rbp
	mov [rcx+80h],rax
	nop 
	nop 
	mov rax,[rdx]
	mov rbx,[rdx+8h]
	mov rcx,[rdx+10h]
	mov rsi,[rdx+20h]
	mov rdi,[rdx+28h]
	mov r8,[rdx+30h]
	mov r9,[rdx+38h]
	mov r10,[rdx+40h]
	mov r11,[rdx+48h]
	mov r12,[rdx+50h]
	mov r13,[rdx+58h]
	mov r14,[rdx+60h]
	mov r15,[rdx+68h]
	mov rbp,[rdx+78h]
	mov rsp,[rdx+70h]
	push 0
	push rax
	mov rax,[rdx+80h]
	mov [rsp+8h],rax
	mov rdx,[rdx+18h]
	pop rax
	ret 
jmps endp
END