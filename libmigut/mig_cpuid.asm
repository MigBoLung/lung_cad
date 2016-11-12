segment .data

segment .bss

segment .text
        global  _mig_cpuid

_mig_cpuid :

	push ebp
	mov ebp , esp

	push ebx
	push esi

	mov eax , [ebp+8]	; level
	cpuid

	mov esi , [ebp+12]	; save regs
	mov[esi+0]  , eax
	mov[esi+4]  , ebx
	mov[esi+8]  , ecx
	mov[esi+12] , edx

	pop esi
	pop ebx
	pop ebp

	ret
