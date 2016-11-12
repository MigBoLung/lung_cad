; void mig_memcpy_mmx ( void *dst , const void *src , int size )

.586
.K3D
.XMM

_TEXT SEGMENT
PUBLIC _mig_memcpy_mmx

_mig_memcpy_mmx PROC NEAR

	push ebp
	mov ebp , esp

	push eax
	push ebx
	push ecx
	push edx
	push esi
	push edi

	; parameters passed into routine:
	; [ebp+8]  = dst
	; [ebp+12] = src
	; [ebp+16] = size

	emms			; empty mmx state

	mov edi , [ebp+ 8]	; dst
	mov esi , [ebp+12]	; src
	mov eax , [ebp+16]	; size

	add edi , eax		; edi ->the end of the last element of dst
	add esi , eax		; esi ->the end of the last element of src

	shr eax , 3            	; bytes to copy / 8 =
                     		; quad words to be copied from src[] to dst[]
	neg eax

	align 16             	; align instructions to 16 byte boundary


copyloop :		; copy 128 bytes of x[] to y[] with each pass
			; through the code below

	movq mm0 , [esi+8*eax]     ; mmx0 = [x(1),x(0)]
	movq mm1 , [esi+8*eax+8]   ; mmx1 = [x(3),x(2)]
	movq mm2 , [esi+8*eax+16]  ; mmx2 = [x(5),x(4)]
	movq mm3 , [esi+8*eax+24]  ; mmx3 = [x(7),x(6)]

	movq [edi+8*eax]    , mm0  ; [y(1),y(0)] = [x(1),x(0)]
	movq [edi+8*eax+8]  , mm1  ; [y(3),y(2)] = [x(3),x(2)]
	movq [edi+8*eax+16] , mm2  ; [y(5),x(4)] = [x(5),x(4)]
	movq [edi+8*eax+24] , mm3  ; [y(7),y(6)] = [x(7),x(6)]

	movq mm4 , [esi+8*eax+32]  ; mmx4 = [x(9),x(8)]
	movq mm5 , [esi+8*eax+40]  ; mmx5 = [x(11),x(10)]
	movq mm6 , [esi+8*eax+48]  ; mmx6 = [x(13),x(12)]
	movq mm7 , [esi+8*eax+56]  ; mmx7 = [x(15),x(14)]

	movq [edi+8*eax+32] , mm4  ; [y(9),y(8)] = [x(9),x(8)]
	movq [edi+8*eax+40] , mm5  ; [y(11),y(10)] = [x(11),x(10)]
	movq [edi+8*eax+48] , mm6  ; [y(13),y(12)] = [x(13),x(12)]
	movq [edi+8*eax+56] , mm7  ; [y(15),y(14)] = [x(15),x(14)]

	movq mm0 , [esi+8*eax+64]  ; mmx0 = [x(17),x(16)]
	movq mm1 , [esi+8*eax+72]  ; mmx1 = [x(19),x(18)]
	movq mm2 , [esi+8*eax+80]  ; mmx2 = [x(21),x(20)]
	movq mm3 , [esi+8*eax+88]  ; mmx3 = [x(23),x(22)]

	movq [edi+8*eax+64] , mm0  ; [y(17),y(16)] = [x(17),x(16)]
	movq [edi+8*eax+72] , mm1  ; [y(19),y(18)] = [x(19),x(18)]
	movq [edi+8*eax+80] , mm2  ; [y(21),x(20)] = [x(21),x(20)]
	movq [edi+8*eax+88] , mm3  ; [y(23),y(22)] = [x(23),x(22)]

	movq mm4 , [esi+8*eax+96]  ; mmx4 = [x(25),x(24)]
	movq mm5 , [esi+8*eax+104] ; mmx5 = [x(27),x(26)]
	movq mm6 , [esi+8*eax+112] ; mmx6 = [x(29),x(28)]
	movq mm7 , [esi+8*eax+120] ; mmx7 = [x(31),x(30)]

	movq [edi+8*eax+96]  , mm4 ; [y(25),y(24)] = [x(25),x(24)]
	movq [edi+8*eax+104] , mm5 ; [y(27),y(26)] = [x(27),x(26)]
	movq [edi+8*eax+112] , mm6 ; [y(29),y(28)] = [x(29),x(28)]
	movq [edi+8*eax+120] , mm7 ; [y(31),y(30)] = [x(31),x(30)]

	add eax , 16

js copyloop

	emms

	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
	mov esp , ebp
	pop ebp

	ret

_mig_memcpy_mmx ENDP
_TEXT ENDS
END
