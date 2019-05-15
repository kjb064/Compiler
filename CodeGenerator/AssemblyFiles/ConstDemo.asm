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
	A	DW 4
	B	DW 5
	C	DW 100

section	.bss	;used to declare uninitialized variables

	TempChar        RESB    1 ;1 byte temp space for use by GetNextChar
	testchar        RESB    1	
	;Temporary storage GetAnInteger.	
	ReadInt         RESW    1 ;4 bytes
	;Used in converting to base ten.
	tempint         RESW	1              
	negflag         RESB    1         ;P=positive, N=negative	
	D	RESW 1
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
main:	mov word [D], 3
	mov ax, [B]
	test al, 1
	jz L1

	mov word [D], 1500
L1:	nop
	mov ax, [D]
	call ConvertIntegerToString

	mov eax, 4
	mov ebx, 1
	mov ecx, Result
	mov edx, ResultEnd
	int 80h

	mov ax, [A]
	add ax, [B]
	mov [T1], ax

	mov ax, [T1]
	add ax, [C]
	mov [D], ax

	mov ax, [D]
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
