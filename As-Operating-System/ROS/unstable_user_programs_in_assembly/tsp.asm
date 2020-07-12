
;*********************************************
;	YEMELITCOS User Program
;
;	tsp.asm
;
;*********************************************
; Content: (Last checked: 21 Nov 2012)
;	-
;	-
;*********************************************

use32

include "../general_info.inc"			; contains no code (only MACROs)

org	PROGRAM_LOCATION

jmp	Start_TSP



N	equ 51
STACK	equ N
CHECK	equ N*100

n		dd 0				; value of n
InputData	dd 0				; pointer to the input data space
LinkedList	dd 0				; pointer to the verteces linked list space
CheckArray	dd 0				; pointer to the check array space
check		dd 0
SStack		dd 0				; pointer to the stack space
stackCounter	dd 0
smallestCost	dd 0				; smallest cost so far
currentCost	dd 0				; current cost of linked list
acc		dd 0



TRUE	equ 1
FALSE	equ 0

; parent structure
pStart		equ 0
pStop		equ 4
pBegin		equ 8
pEnd		equ 12
pBehind		equ 16
pInfront	equ 20
pClockwise	equ 24	; a boolean (1-byte)
pSize		equ 25



Start_TSP:
	pusha

	mov dword [InputData], ENDofPROGRAM
	call	loadInputData
;	call	printInputData

	mov	eax, [n]
	shl	eax, 3				; an element in InputData takes 2*4 bytes
	add	eax, [InputData]
	mov	[LinkedList], eax		; linked list space just after input data space

	mov	ecx, [n]
	add	ecx, [n]
	add	ecx, [n]
	shl	ecx, 2				; ecx = size of linked list
	add	eax, ecx
	mov	[CheckArray], eax		; check array space just after linked list space

	mov	ecx, CHECK			; CHECK = CheckArray size
	shl	ecx, 3				; an element of CheckArray takes 2*4 bytes
	add	eax, ecx
	mov	[SStack], eax			; SStack space just after check array space

	popa



startOf_SolveTSP:
	pusha


.initialise:
	xor	eax, eax
	mov	ebx, [LinkedList]
	mov	edi, ebx
	add	edi, 12
	mov	[ebx+4], edi				; set first vertex
	mov	[ebx+8], eax

	mov	ecx, [n]
	dec	ecx
   .loop_initialise:					; set verteces in-between start and last
	inc	eax
	mov	esi, ebx
	add	ebx, 12
	mov	edi, ebx
	add	edi, 12

	mov	[ebx], esi
	mov	[ebx+4], edi
	mov	[ebx+8], eax
	loop	.loop_initialise

	mov	edi, [LinkedList]
	mov	[ebx+4], edi				; set last vertex
	mov	[edi], ebx

;	call	loadAnswer
	call	printCycle



.someFirstSteps:
	xor	edx, edx
	mov	ebx, [LinkedList]
	mov	ecx, [n]
   .loop:
	push dword [ebx+4]
	push	ebx
	call	getEdgeCost				; eax = getEdgeCost(ebx, ebx->next)
	add	edx, eax
	pop	ebx
	pop	ebx					; ebx = ebx->next
	loop	.loop

	mov	[currentCost], edx
	mov	[smallestCost], edx
	call	printSmallestCost

	mov dword [acc], 0
	mov	ebx, [LinkedList]



.startPrimaryLoop:
	mov	[.temp], ebx
	call	printacc
	inc dword [acc]

	mov	[.start], ebx		; start = temp
	mov	edi, [ebx]		; end = start->prev
	mov	ebx, [ebx+4]
	mov	[.stop], ebx		; stop = temp2->next
	mov	esi, [ebx+4]		; begin = stop->next
	mov byte [.clockwise], TRUE
	mov dword [stackCounter], 0
	mov dword [check], 0


	.startSecondaryLoop:
	.forBegins:					; for( ; begin!=start; begin = begin->next)
		cmp	esi, [.start]
		je	.noteIsFalse
		.forEnds:
			cmp	edi, [.stop]
			je	.after_forEnds
			cmp	esi, edi		; if(begin==end) clockwise=FALSE;
			jne	.cont_forEnds_1
			mov byte [.clockwise], FALSE

		   .cont_forEnds_1:
			cmp byte [.clockwise], TRUE
			jne	.cont_forEnds_1_CCW
		   .cont_forEnds_1_CW:				; clockwise = TRUE
			mov	ecx, [esi]			; ecx = behind = begin->prev;
			push	esi
			push	ecx
			call	getEdgeCost			; eax = getEdgeCost(behind, begin)
			mov	ebx, eax
			mov	edx, [edi+4]			; edx = infront = end->next;
			push	edx
			push	edi
			call	getEdgeCost			; eax = getEdgeCost(end, infront)
			add	ebx, eax			; sum1 = getEdgeCost(behind, begin) + getEdgeCost(end, infront);
			jmp	.cont_forEnds_2
		   .cont_forEnds_1_CCW:				; clockwise = FALSE
			mov	ecx, [edi]			; ecx = behind = end->prev;
			push	edi
			push	ecx
			call	getEdgeCost			; eax = getEdgeCost(behind, end)
			mov	ebx, eax
			mov	edx, [esi+4]			; edx = infront = begin->next;
			push	edx
			push	esi
			call	getEdgeCost			; eax = getEdgeCost(begin, infront)
			add	ebx, eax			; sum1 = getEdgeCost(behind, end) + getEdgeCost(begin, infront);

		   .cont_forEnds_2:
			add	esp, 16
			mov	[.sum1], ebx
			mov	[.behind], ecx
			mov	[.infront], edx

			push	edx				; save infront (for temp2 later)
			push	ecx				; save behind (for temp1 later)
			call	getEdgeCost
			mov	edx, eax			; edx = k = getEdgeCost(behind, infront);
			push	esi
			push dword [.start]
			call	getEdgeCost
			mov	ebx, eax			; ebx = i = getEdgeCost(start, begin);
			push dword [.stop]
			push	edi
			call	getEdgeCost
			mov	ecx, eax			; ecx = j = getEdgeCost(end, stop);
			add	esp, 16				; is 16 and not 24, remember what we saved!

			cmp	ebx, ecx
			jbe	.i_lessOrEqual_j
			cmp	ebx, edx
			jbe	.i_less_k_greater_j
			.k_less_i_greater_j:
				mov	eax, ecx
				add	eax, edx			; eax = sum1 = j+k
				add	esp, 8
				push	esi				; temp1=start; temp2=begin;
				push dword [.start]
				jmp	.cont_forEnds_3
			.i_lessOrEqual_j:
			cmp	ecx, edx
			jbe	.i_less_k_greater_j
			.i_less__j_greater_k:
				mov	eax, ebx
				add	eax, edx			; eax = sum1 = i+k
				add	esp, 8
				push dword [.stop]			; temp1=end; temp2=stop;
				push	edi
				jmp	.cont_forEnds_3
			.i_less_k_greater_j:
				mov	eax, ebx
				add	eax, ecx			; eax = sum1 = i+j

		   .cont_forEnds_3:
			cmp	eax, [.sum1]				; cmp sum2, sum1
			ja	.cont_forEnds_4
			call	checkIfUsed				; temp1 and temp2 are both already in stack!
			jnc	.noteIsTrue				; break from the nested 'for' loops

		   .cont_forEnds_4:
			add	esp, 8
			mov	edi, [edi]			; end = end->prev
			jmp	.forEnds
		.after_forEnds:
		mov	esi, [esi+4];				; begin = begin->next
		mov	edi, [.start]
		mov	edi, [edi]				; end = start->prev;
		mov byte [.clockwise], TRUE
		jmp	.forBegins


	.noteIsFalse:
		mov	eax, [stackCounter]
		cmp	eax, 0
		jbe	.iteratePrimaryLoop

		dec	eax
		mov	[stackCounter], eax
		mov	ebx, [SStack]			; ebx = parent
		mov	ecx, pSize
		mul	ecx
		add	ebx, eax			; ebx = [SStack] + stackCounter*pSize

		movzx	ax, byte [ebx+pClockwise]
		mov	[.clockwise], al		; clockwise = parent->clockwise
		push	ax
		mov	eax, [ebx+pInfront]
		mov	[.stop], eax			; stop = parent->infront
		push	eax
		mov	eax, [ebx+pBehind]
		mov	[.start], eax			; start = parent->behind
		push	eax
		push dword [ebx+pStop]
		push dword [ebx+pStart]
		call	transformCycle
		add	esp, 18

		mov	esi, [ebx+pBegin]		; begin = parent->begin
		mov	edi, [ebx+pEnd]			; end = parent->end
;		mov	edi, [edi]
		jmp	.startSecondaryLoop


	.noteIsTrue:
		movzx	ax, byte [.clockwise]
		push	ax
		push dword [.infront]
		push dword [.behind]
		push dword [.stop]
		push dword [.start]
		call	transformCycle
		add	esp, 18

		mov	eax, [currentCost]
		cmp	eax, [smallestCost]
		jae	.cont1_noteIsTrue

		add	esp, 8
		mov	[smallestCost], eax
		call	printCycle
		call	printSmallestCost
		mov dword [acc], 0
		mov	ebx, [LinkedList]
		jmp	.startPrimaryLoop

	.cont1_noteIsTrue:
		mov	eax, [stackCounter]
		inc	eax
		cmp	eax, STACK
		jbe	.cont2_noteIsTrue

		add	esp, 8
		mov	esi, .stackOverflow
		call	printstr
		jmp	.endOf_SolveTSP

	.cont2_noteIsTrue:
		mov	[stackCounter], eax
		dec	eax
		mov	ebx, [SStack]			; esi = parent
		mov	ecx, pSize
		mul	ecx
		add	ebx, eax			; ebx = [SStack] + stackCounter*pSize

		mov	eax, [.behind]
		mov	[ebx+pStart], eax		; parent->start = behind
		mov	eax, [.infront]
		mov	[ebx+pStop], eax		; parent->stop = infront
		mov	eax, [.start]
		mov	[ebx+pBehind], eax		; parent->behind = start
		mov	eax, [.stop]
		mov	[ebx+pInfront], eax		; parent->infront = stop
		mov	[ebx+pBegin], esi		; parent->begin = begin
		mov	[ebx+pEnd], edi			; parent->end = end
		mov	al, [.clockwise]
		mov	[ebx+pClockwise], al		; parent->clockwise = clockwise

		pop	ecx
		pop	edx
		mov	[.start], ecx		; start = temp1
		mov	edi, [ecx]		; end = start->prev
		mov	[.stop], edx		; stop = temp2
		mov	esi, [edx+4]		; begin = stop->next
		mov byte [.clockwise], TRUE
		jmp	.startSecondaryLoop


.iteratePrimaryLoop:
	mov	ebx, [.temp]
	mov	ebx, [ebx+4]
	cmp	ebx, [LinkedList]
	jne	.startPrimaryLoop

.endOf_SolveTSP:
	popa
	ret

   .sum1 dd 0
   .temp dd 0
   .start dd 0
   .stop dd 0
   .behind dd 0			; note: differentiate with temp1
   .infront dd 0		; note: differentiate with temp2
   .clockwise db 0

   .stackOverflow db 13, "stack overflow", 13, 13, 0



printacc:
	push	eax
	mov	al, 'a'
	int	0x74			; print character
	mov	al, 'c'
	int	0x74
	mov	al, 'c'
	int	0x74
	mov	al, '='
	int	0x74
	mov	eax, [acc]
	call	printint
	mov	al, 13
	int	0x74
	pop	eax
	ret



printSmallestCost:
	push	eax
	push	esi
	mov	esi, .msgSmallestCost
	call	printstr
	mov	eax, [smallestCost]
	call	printint
	mov	al, 13
	int	0x74
	pop	esi
	pop	eax
	ret
   .msgSmallestCost	db 13, "   smallestCost = ", 0



;----------------------------------------------------------------
; void transformCycle (start, stop, behind, infront, clockwise)
;	inputs pushed to stack
;----------------------------------------------------------------
transformCycle:
	push	ebp
	mov	ebp, esp
	add	ebp, 8
	pusha

	mov	ecx, [ebp+8]			; get behind
	mov	edx, [ebp+12]			; get infront

	cmp byte [ebp+16], TRUE			; check if clockwise
	jne	.counterclockwise

.clockwise:
	mov	esi, [ecx+4]			; esi = begin = behind->next
	mov	edi, [edx]			; edi = end = infront->prev

   .decrements_CW:
	push dword [ebp+4]
	push dword [ebp]
	call	getEdgeCost			; eax = getEdgeCost (start, stop)
	mov	ebx, eax
	push	esi
	push	ecx
	call	getEdgeCost			; eax = getEdgeCost (behind, begin)
	add	ebx, eax
	push	edx
	push	edi
	call	getEdgeCost			; eax = getEdgeCost (end, infront)
	add	ebx, eax
	sub	[currentCost], ebx
   .increments_CW:
	push	esi
	push dword [ebp]
	call	getEdgeCost			; eax = getEdgeCost (start, begin)
	mov	ebx, eax
	push dword [ebp+4]
	push	edi
	call	getEdgeCost			; eax = getEdgeCost (end, stop)
	add	ebx, eax
	push	edx
	push	ecx
	call	getEdgeCost			; eax = getEdgeCost (behind, infront)
	add	ebx, eax
	add	[currentCost], ebx
   add esp, 48

   .transform_CW:
	mov	eax, [ebp]			; get start
	mov	[eax+4], esi			; start->next = begin
	mov	[esi], eax			; begin->prev = start
	mov	ebx, [ebp+4]			; get stop
	mov	[ebx], edi			; stop->prev = end
	mov	[edi+4], ebx			; end->next = stop
	mov	[ecx+4], edx			; behind->next = infront
	mov	[edx], ecx			; infront->prev = behind
	jmp	.done

.counterclockwise:
	mov	edi, [ecx+4]			; edi = end = behind->next
	mov	esi, [edx]			; esi = begin = infront->prev

   .decrements_CCW:
	push dword [ebp+4]
	push dword [ebp]
	call	getEdgeCost			; eax = getEdgeCost (start, stop)
	mov	ebx, eax
	push	edi
	push	ecx
	call	getEdgeCost			; eax = getEdgeCost (behind, end)
	add	ebx, eax
	push	edx
	push	esi
	call	getEdgeCost			; eax = getEdgeCost (begin, infront)
	add	ebx, eax
	sub	[currentCost], ebx
   .increments_CCW:
	push	esi
	push dword [ebp]
	call	getEdgeCost			; eax = getEdgeCost (start, begin)
	mov	ebx, eax
	push dword [ebp+4]
	push	edi
	call	getEdgeCost			; eax = getEdgeCost (end, stop)
	add	ebx, eax
	push	edx
	push	ecx
	call	getEdgeCost			; eax = getEdgeCost (behind, infront)
	add	ebx, eax
	add	[currentCost], ebx
   add esp, 48

   .transform_CCW:
	mov	eax, [ebp]			; get start
	mov	[eax+4], esi			; start->next = begin
	mov	[esi+4], eax			; begin->next = start	// will be fixed during spinning
	mov	ebx, [ebp+4]			; get stop
	mov	[ebx], edi			; stop->prev = end
	mov	[edi], ebx			; end->prev = stop	// will be fixed during spinning
	mov	[ecx+4], edx			; behind->next = infront
	mov	[edx], ecx			; infront->prev = behind

	mov	ecx, ebx			; get stop
   .spinning:					; the vertex represented with esi is to be spinned
	cmp	esi, ecx
	je	.done
	mov	eax, [esi]			; eax = esi->prev
	mov	ebx, [esi+4]			; ebx = esi->next
	mov	[esi], ebx
	mov	[esi+4], eax
	mov	esi, eax
	jmp	.spinning			; while esi != stop

   .done:
	popa
	pop	ebp
	ret



;----------------------------------------------------
; cf=bool checkIfUsed (from, to)
;----------------------------------------------------
checkIfUsed:
	push	ebp
	mov	ebp, esp
	add	ebp, 8
	pusha

	mov	esi, [ebp]
	mov	esi, [esi+8]			; esi = from->value
	mov	edi, [ebp+4]
	mov	edi, [edi+8]			; edi = to->value

	mov	ecx, [check]
	push	ecx
	mov	ebx, [CheckArray]
   .loop:
	cmp	ecx, 0
	je	.notFound
	cmp	[ebx], esi			; check if from vertex = first dword
	jne	.cont
	cmp	[ebx+4], edi			; check if to vertex = second dword
	jne	.cont
	pop	ecx
	stc
	jmp	.done
   .cont:
	add	ebx, 8				; move to next element in CheckArray (skip 8 bytes)
	loop	.loop

  .notFound:
	mov	[ebx], esi			; store new Edge
	mov	[ebx+4], edi
	pop	ecx
	inc	ecx
	mov	[check], ecx
	cmp	ecx, CHECK
	jbe	.fine
	mov	esi, .checkOverflow
	call	printstr
	jmp	$
   .fine:
	clc
   .done:
	popa
	pop	ebp
	ret

   .checkOverflow db 13, "check overflow", 13, 13, 0



;----------------------------------------------------
; eax=int EdgeCost (from, to)
;----------------------------------------------------
getEdgeCost:
	push	ebp
	mov	ebp, esp
	add	ebp, 8
	pusha

	mov	esi, [ebp]
	mov	esi, [esi+8]			; esi = from->value
	mov	edi, [ebp+4]
	mov	edi, [edi+8]			; edi = to->value

	mov	ebx, esi
	shl	ebx, 3
	add	ebx, [InputData]
	push	ebx				; save, to be used later to get Y(from)
	mov	eax, [ebx]			; get X(from)

	mov	ebx, edi
	shl	ebx, 3
	add	ebx, [InputData]
	push	ebx				; save, to be used later to get Y(to)
	sub	eax, [ebx]			; get X(to), and substract

	mul	eax				; eax*eax
	mov	ecx, eax

	pop	ebx
	mov	eax, [ebx+4]			; get Y(to)
	pop	ebx
	sub	eax, [ebx+4]			; get Y(from)

	mul	eax				; eax*eax
	add	ecx, eax			; ecx = dx*dx + dy*dy

	mov	[.temp], ecx
;	fild dword [.temp]
;	fsqrt			; compute square root
;	fistp dword [.temp]	; get nearest integer approximation

	popa
	pop	ebp
	mov	eax, [.temp]
	ret
   .temp dd 0



printCycle:
	pusha
	mov	al, 13		; newline character
	int	0x74
	mov	al, '['
	int	0x74
	mov	al, ' '
	int	0x74
	mov	esi, [LinkedList]
	mov	ecx, [n]
   .loop:
	mov	eax, [esi+8]	; get vertex value
	call	printint
	mov	al, ' '
	int	0x74
	mov	esi, [esi+4]	; get next vertex
	loop	.loop
	mov	al, ']'
	int	0x74
	mov	al, 13		; newline character
	int	0x74
	popa
	ret



loadAnswer:
	pusha
	mov	eax, [n]		; these four lines
	shl	eax, 2			; are for the purpose
	add	eax, 4			; of skipping the InputData
	add	eax, 0x41000		; found inside the loaded file
	mov	esi, eax
	mov	edi, [LinkedList]
	mov	ecx, [n]
   .loop:
	add	edi, 8
	movsd
	loop	.loop
	popa
	ret



loadInputData:
	pusha

;	mov	esi, .tspFileName
;	mov	edi, [InputData]
;	call dword [_LoadFile]
;	mov	eax, [edi]			; these three lines are
;	mov	[n], eax			; because the binary file
;	add dword [InputData], 4		; starts with the value of n
;	.............
;	popa
;	ret

	mov	esi, 0x41000
	lodsd					; notice where 'esi' is left after this operation
	mov	[n], eax
	cmp	eax, N
	jbe	.cont

	mov	esi, .nTooLarge_0
	call	printstr
	mov	eax, N
	call	printint
	mov	esi, .nTooLarge_1
	call	printstr
	mov	eax, [n]
	call	printint
	mov	esi, .nTooLarge_2
	call	printstr
	hlt
	jmp	$

   .cont:
	xor	eax, eax
	mov	ecx, [n]
	mov	edi, [InputData]
   .loop:
	lodsw				; yes, load a word
	stosd				; but, store a dword
	lodsw
	stosd
	loop	.loop

	popa
	ret

   .tspFileName db "graph.tsp", 0
   .nTooLarge_0 db 13, "Error. Maximun number of verteces is ", 0
   .nTooLarge_1 db " , but n = ", 0
   .nTooLarge_2 db " .", 13, 0



printInputData:
	pusha

	mov	al, 13			; newline character
	int	0x74
	mov	eax, [n]
	call	printint
	mov	al, 13
	int	0x74

	mov	esi, [InputData]
	mov	ecx, [n]
	mov	ebx, 0
   .loop:
	inc	ebx
	mov	eax, ebx
	call	printint

	mov	al, ' '
	int	0x74
	lodsd				; load x value
	call	printint

	mov	al, ' '
	int	0x74
	lodsd				; load y value
	call	printint

	mov	al, 13			; newline character
	int	0x74
	loop	.loop
   .done:
	popa
	ret



;------------------------------------------
;   print integer number
;	in: eax = value
;------------------------------------------

printint:
	pusha

	mov	edi, 9
	xor	edx, edx
	mov	ecx, 10
	div	ecx				; eax /= 10
	mov	esi, edx			; get remainder,
	and	esi, 0x0000000F			; which is inside dl
	mov	dl, [.digitsList+esi]
	mov	[.intNumber+edi], dl

	mov	ebx, 9
	mov	ecx, 9
   .loop:
	push	ecx
	dec	edi
	xor	edx, edx
	mov	ecx, 10
	div	ecx				; eax /= 10
	cmp	dl, 0
	je	.cont
	mov	ebx, edi			; update ebx
   .cont:
	mov	esi, edx			; get remainder,
	and	esi, 0x0000000F			; which is inside dl
	mov	dl, [.digitsList+esi]
	mov	[.intNumber+edi], dl
	pop	ecx
	loop	.loop

   .done:
	mov	esi, .intNumber
	add	esi, ebx
	call	printstr
	popa
	ret
   .digitsList	db '0123456789'
   .intNumber	db '1234567890', 0


;------------------------------------------
;   print hexadecimal number
;	in: eax = value
;------------------------------------------

printhex:
	pusha

	mov	edi, 7
	mov	si, ax
	and	esi, 0x0000000F			; get al
	mov	dl, [.digitsList+esi]
	mov	[.hexNumber+edi], dl

	mov	ebx, 7
	mov	ecx, 7
   .loop:
	dec	edi
	shr	eax, 4
	cmp	al, 0
	je	.cont
	mov	ebx, edi			; update ebx
   .cont:
	mov	si, ax
	and	esi, 0x0000000F			; get al
	mov	dl, [.digitsList+esi]
	mov	[.hexNumber+edi], dl
	loop	.loop

   .done:
	mov	esi, .hexNumber
	add	esi, ebx
	call	printstr
	popa
	ret
   .digitsList	db '0123456789ABCDEF'
   .hexNumber	db '12345678', 'h', 0


;----------------------------------------------------------
;   print a string, of 1-byte characters, to screen
;	in: esi = address of string
;----------------------------------------------------------
printstr:
	push	ax
	push	esi
   .loop:
	lodsb
	or	al, al				; check if character==0
	jz	.done
	int	0x74				; print character
	jmp	.loop
   .done:
	pop	esi
	pop	ax
	ret


ENDofPROGRAM:

