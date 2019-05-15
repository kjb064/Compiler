sys_exit	equ	1
sys_read	equ	3
sys_write	equ	4
stdin		equ	0 ; default keyboard
stdout		equ	1 ; default terminal screen
stderr		equ	3

section .data		;used to declare constants	
	userMsg		db  'Enter an integer(less than 32,765): '
	lenUserMsg	equ	$-userMsg
	displayMsg	db	'You entered: '
	lenDisplayMsg	equ	$-displayMsg
	newline		db	0xA 	; 0xA 0xD is ASCII <LF><CR>

	Ten        DW      10  ;Used converting to base ten.
	Result          db      'Ans = '
	ResultValue		 db	'aaaaa'
			        db	0xA		;return
	ResultEnd       equ 	$-Result   ; $=> here - address Result = length to print
	num			times 6	 db 'ABCDEF' ;cheat NASM
	numEnd		equ	$-num
	M	DW 7
	N	DW 85
	LIT0	DW 0
	LIT2	DW 2
	LIT1	DW 1
	LIT25	DW 25
	LIT3	DW 3
	LIT84	DW 84
	LIT36	DW 36

section	.bss	;used to declare uninitialized variables

	TempChar        RESB    1 ;1 byte temp space for use by GetNextChar
	testchar        RESB    1	
	;Temporary storage GetAnInteger.	
	ReadInt         RESW    1 ;4 bytes
	;Used in converting to base ten.
	tempint         RESW	1              
	negflag         RESB    1         ;P=positive, N=negative	
	X	RESW 1
	Y	RESW 1
	Z	RESW 1
	Q	RESW 1
	R	RESW 1
	A	RESW 1
	B	RESW 1
	W	RESW 1
	F	RESW 1
	G	RESW 1
	T1	RESW 1
	T2	RESW 1
	T3	RESW 1
	T4	RESW 1
	T5	RESW 1
	T6	RESW 1
	T7	RESW 1
	T8	RESW 1

section .txt

   global main
main:	mov ax, [M]
	mov [X], ax

	mov ax, [N]
	mov [Y], ax

	call Multiply
	mov ax, [LIT25]
	mov [X], ax

	mov ax, [LIT3]
	mov [Y], ax

	call Divide
	mov ax, [LIT84]
	mov [X], ax

	mov ax, [LIT36]
	mov [Y], ax

	call GCD
	mov ax, [Z]
	call ConvertIntegerToString

	mov eax, 4
	mov ebx, 1
	mov ecx, Result
	mov edx, ResultEnd
	int 80h

fini:
	mov eax, sys_exit
	xor ebx, ebx
	int 80h

Multiply:	nop
	mov ax, [X]
	mov [A], ax

	mov ax, [Y]
	mov [B], ax

	mov ax, [LIT0]
	mov [Z], ax

W1:	nop
	mov ax, [B]
	cmp ax, [LIT0]
	jle L1
	mov ax, [B]
	test al, 1
	jz L2

	mov ax, [Z]
	add ax, [A]
	mov [Z], ax

L2:	nop
	mov ax, [LIT2]
	mul word [A]
	mov [A], ax

	mov dx, 0
	mov ax, [B]
	mov bx, [LIT2]
	div bx
	mov [B], ax

	jmp W1
L1:	nop
	ret
Divide:	nop
	mov ax, [X]
	mov [R], ax

	mov ax, [LIT0]
	mov [Q], ax

	mov ax, [Y]
	mov [W], ax

W2:	nop
	mov ax, [W]
	cmp ax, [R]
	jg L3
	mov ax, [LIT2]
	mul word [W]
	mov [W], ax

	jmp W2
L3:	nop
W3:	nop
	mov ax, [W]
	cmp ax, [Y]
	jle L4
	mov ax, [LIT2]
	mul word [Q]
	mov [Q], ax

	mov dx, 0
	mov ax, [W]
	mov bx, [LIT2]
	div bx
	mov [W], ax

	mov ax, [W]
	cmp ax, [R]
	jg L5

	mov ax, [R]
	sub ax, [W]
	mov [R], ax

	mov ax, [Q]
	add ax, 1
	mov [Q], ax

L5:	nop
	jmp W3
L4:	nop
	ret
GCD:	nop
	mov ax, [X]
	mov [F], ax

	mov ax, [Y]
	mov [G], ax

W4:	nop
	mov ax, [F]
	cmp ax, [G]
	je L6
	mov ax, [F]
	cmp ax, [G]
	jge L7

	mov ax, [G]
	sub ax, [F]
	mov [G], ax

L7:	nop
	mov ax, [G]
	cmp ax, [F]
	jge L8

	mov ax, [F]
	sub ax, [G]
	mov [F], ax

L8:	nop
	jmp W4
L6:	nop
	mov ax, [F]
	mov [Z], ax

	ret
;PrintString     PROC
PrintString:
	push    ax              ;Save registers;
	push    dx
; subpgm:
	; prompt user	
	mov eax, 4		;Linux print device register conventions
	mov ebx, 1		; print default output device
	mov ecx, userMsg	; pointer to string
	mov edx, lenUserMsg	
	int	80h		; interrupt 80 hex, call kernel
	pop     dx              ;Restore registers.
	pop     ax
	ret
;PrintString     ENDP

;GetAnInteger    PROC

GetAnInteger:	;Get an integer as a string
	;get response
	mov eax,3	;read
	mov ebx,2	;device
	mov ecx,num	;buffer address
	mov edx,6	;max characters
	int 0x80

	;print number    ;works
	mov edx,eax 	; eax contains the number of 
			; character read including <lf>
	mov eax, 4
	mov ebx, 1
	mov ecx, num
	int 80h

ConvertStringToInteger:
	mov ax,0	;hold integer
	mov [ReadInt],ax ;initialize 16 bit number to zero
	mov ecx,num	;pt - 1st or next digit of number as a string 
				;terminated by <lf>.
	mov bx,0	
	mov bl, byte [ecx] ;contains first or next digit
Next:	sub bl,'0'	;convert character to number
	mov ax,[ReadInt]
	mov dx,10
	mul dx		;eax = eax * 10
	add ax,bx
	mov [ReadInt], ax

	mov bx,0
	add ecx,1 	;pt = pt + 1
	mov bl, byte[ecx]

	cmp bl,0xA	;is it a <lf>
	jne Next	; get next digit   
	ret
;	ENDP GetAnInteger

;ConvertIntegerToString PROC

ConvertIntegerToString:
	mov ebx, ResultValue + 4   ;Store the integer as a
				   ; five digit char string at Result for printing

ConvertLoop:
	sub dx,dx  ; repeatedly divide dx:ax by 10
		   ; to obtain last digit of number
	mov cx,10  ; as the remainder in the DX
		   ; register.  Quotient in AX.
	div cx
	add dl,'0' ; Add '0' to dl to 
		   ;convert from binary to character.
	mov [ebx], dl
	dec ebx
	cmp ebx,ResultValue
	jge ConvertLoop

	ret

;ConvertIntegerToString  ENDP
