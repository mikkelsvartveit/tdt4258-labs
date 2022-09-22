.global _start

.section .text

_start:
	b check_input

	
check_input:
	// Let r0 hold the address of the character to read
	ldr r0, =input
	// Let r1 hold input length
	mov r1, #0
	
	count_loop:
		// Compute address to next character
		add r0, r0, #1
		// Load next single character from string
		ldrb r2, [r0]
		
		// Increment input length
		add r1, r1, #1
		
		// Check if the character ASCII value is 0 (indicates end of string)
		cmp r2, #0
		// Repeat if character is not 0
		bne count_loop
	
	b check_palindrome
	

check_palindrome:
	// Prepare counters
	// r2 counts up from start of string, r1 counts down from end of string
	mov r2, #0
	sub r1, r1, #1
	
	// Load address of first character
	ldr r0, =input
	
	compare_loop:
		// Load characters to r4 and r5
		ldrb r4, [r0, r2]
		ldrb r5, [r0, r1]
		
		// Check for spaces in character 1
		cmp r4, #0x20
		bne skip_remove_space_1
		// If space detected, increase counter and run again
		add r2, r2, #1
		b compare_loop
		skip_remove_space_1:
		
		// Check for spaces in character 2
		cmp r5, #0x20
		bne skip_remove_space_2
		// If space detected, decrease counter and run again
		sub r1, r1, #1
		b compare_loop
		skip_remove_space_2:
		
		// Compare counters
		cmp r2, r1
		// If counters have "met" in the middle, we are done
		bgt palindrome_found
		
		// If lowercase, convert characters to uppercase
		cmp r4, #97
		blt skip_convert_char_1
		sub r4, r4, #32
		skip_convert_char_1:
		cmp r5, #97
		blt skip_convert_char_2
		sub r5, r5, #32
		skip_convert_char_2:

		// Compare the characters
		cmp r4, r5
		// If not equal, phrase is not a palindrome
		bne palindrome_not_found
		
		// Update counters
		add r2, r2, #1
		sub r1, r1, #1
		
		// Repeat loop
		b compare_loop
	
	
palindrome_found:
	// Load address for LEDs to r0
	ldr r0, =0xff200000
	// Load LED pattern to r1
	mov r1, #0b0000011111
	// Set LED pattern
	str r1, [r0]
	
	// Load UART address to r0
	ldr r0, =0xFF201000
	// Send characters to UART
	mov r1, #0x50
	str r1, [r0]
	mov r1, #0x61
	str r1, [r0]
	mov r1, #0x6c
	str r1, [r0]
	mov r1, #0x69
	str r1, [r0]
	mov r1, #0x6e
	str r1, [r0]
	mov r1, #0x64
	str r1, [r0]
	mov r1, #0x72
	str r1, [r0]
	mov r1, #0x6f
	str r1, [r0]
	mov r1, #0x6d
	str r1, [r0]
	mov r1, #0x65
	str r1, [r0]
	mov r1, #0x20
	str r1, [r0]
	mov r1, #0x64
	str r1, [r0]
	mov r1, #0x65
	str r1, [r0]
	mov r1, #0x74
	str r1, [r0]
	mov r1, #0x65
	str r1, [r0]
	mov r1, #0x63
	str r1, [r0]
	mov r1, #0x74
	str r1, [r0]
	mov r1, #0x65
	str r1, [r0]
	mov r1, #0x64
	str r1, [r0]
	mov r1, #0x0a
	str r1, [r0]

	b exit
	
	
palindrome_not_found:
	// Load address for LEDs to r0
	ldr r0, =0xff200000
	// Load LED pattern to r1
	mov r1, #0b1111100000
	// Set LED pattern
	str r1, [r0]
	
	// Load UART address to r0
	ldr r0, =0xff201000
	// Send characters to UART
	mov r1, #0x4e
	str r1, [r0]
	mov r1, #0x6f
	str r1, [r0]
	mov r1, #0x74
	str r1, [r0]
	mov r1, #0x20
	str r1, [r0]
	mov r1, #0x61
	str r1, [r0]
	mov r1, #0x20
	str r1, [r0]
	mov r1, #0x70
	str r1, [r0]
	mov r1, #0x61
	str r1, [r0]
	mov r1, #0x6c
	str r1, [r0]
	mov r1, #0x69
	str r1, [r0]
	mov r1, #0x6e
	str r1, [r0]
	mov r1, #0x64
	str r1, [r0]
	mov r1, #0x72
	str r1, [r0]
	mov r1, #0x6f
	str r1, [r0]
	mov r1, #0x6d
	str r1, [r0]
	mov r1, #0x65
	str r1, [r0]
	mov r1, #0x0a
	str r1, [r0]

	b exit
	
	
exit:
	// Branch here for exit
	b exit
	

.section .data
.align
	// This is the input you are supposed to check for a palindrome
	// You can modify the string during development, however you
	// are not allowed to change the label 'input'!
	input: .asciz "level"
	// input: .asciz "8448"
	// input: .asciz "KayAk"
	// input: .asciz "step on no pets"
	// input: .asciz "Never odd or even"


.end