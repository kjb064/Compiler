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

section	.bss	;used to declare uninitialized variables

	TempChar        RESB    1 ;1 byte temp space for use by GetNextChar
	testchar        RESB    1	
	;Temporary storage GetAnInteger.	
	ReadInt         RESW    1 ;4 bytes
	;Used in converting to base ten.
	tempint         RESW	1              
	negflag         RESB    1         ;P=positive, N=negative	
	ans	RESW 1
	a	RESW 1
	Bob	RESW 1
	Jane	RESW 1
	b	RESW 1
	c	RESW 1
	N	RESW 1
	fact	RESW 1
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
main:	call PrintString
	call GetAnInteger
	mov ax, [ReadInt]
	mov [a], ax

	call PrintString
	call GetAnInteger
	mov ax, [ReadInt]
	mov [b], ax

	call PrintString
	call GetAnInteger
	mov ax, [ReadInt]
	mov [c], ax

	call PrintString
	call GetAnInteger
	mov ax, [ReadInt]
	mov [Bob], ax

	call PrintString
	call GetAnInteger
	mov ax, [ReadInt]
	mov [Jane], ax

	mov ax, [Bob]
	add ax, [Jane]
	mov [T1], ax

	mov ax, [T1]
	sub ax, 10
	mov [T1], ax

	mov dx, 0
	mov ax, [T1]
	mov bx, 2
	div bx
	mov [T1], ax

	mov ax, [T1]
	mov bx, 4
	mul word bx
	mov [T1], ax

	mov ax, [a]
	mul word [T1]
	mov [T1], ax

	mov ax, [b]
	add ax, [c]
	mov [T2], ax

	mov dx, 0
	mov ax, [T1]
	mov bx, [T2]
	div bx
	mov [ans], ax

	mov ax, [ans]
	call ConvertIntegerToString

	mov eax, 4
	mov ebx, 1
	mov ecx, Result
	mov edx, ResultEnd
	int 80h

	call PrintString
	call GetAnInteger
	mov ax, [ReadInt]
	mov [a], ax

	call PrintString
	call GetAnInteger
	mov ax, [ReadInt]
	mov [b], ax

	mov ax, [a]
	cmp ax, [b]
	jle L1

	mov ax, [a]
	call ConvertIntegerToString

	mov eax, 4
	mov ebx, 1
	mov ecx, Result
	mov edx, ResultEnd
	int 80h

L1:	nop
	mov ax, [b]
	cmp ax, [a]
	jle L2

	mov ax, [b]
	call ConvertIntegerToString

	mov eax, 4
	mov ebx, 1
	mov ecx, Result
	mov edx, ResultEnd
	int 80h

L2:	nop
	call PrintString
	call GetAnInteger
	mov ax, [ReadInt]
	mov [a], ax

	call PrintString
	call GetAnInteger
	mov ax, [ReadInt]
	mov [b], ax

	call PrintString
	call GetAnInteger
	mov ax, [ReadInt]
	mov [c], ax

	mov ax, [a]
	cmp ax, [b]
	jle L3

	mov ax, [a]
	cmp ax, [c]
	jle L4

	mov ax, [a]
	call ConvertIntegerToString

	mov eax, 4
	mov ebx, 1
	mov ecx, Result
	mov edx, ResultEnd
	int 80h

L4:	nop
L3:	nop
	mov ax, [b]
	cmp ax, [a]
	jle L5

	mov ax, [b]
	cmp ax, [c]
	jle L6

	mov ax, [b]
	call ConvertIntegerToString

	mov eax, 4
	mov ebx, 1
	mov ecx, Result
	mov edx, ResultEnd
	int 80h

L6:	nop
L5:	nop
	mov ax, [c]
	cmp ax, [a]
	jle L7

	mov ax, [c]
	cmp ax, [b]
	jle L8

	mov ax, [c]
	call ConvertIntegerToString

	mov eax, 4
	mov ebx, 1
	mov ecx, Result
	mov edx, ResultEnd
	int 80h

L8:	nop
L7:	nop
	mov word [fact], 1
	call PrintString
	call GetAnInteger
	mov ax, [ReadInt]
	mov [N], ax

W1:	nop
	mov ax, [N]
	cmp ax, 1
	jle L9
	mov ax, [fact]
	mul word [N]
	mov [fact], ax

	mov ax, [N]
	sub ax, 1
	mov [N], ax

	jmp W1
L9:	nop
	mov ax, [fact]
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
