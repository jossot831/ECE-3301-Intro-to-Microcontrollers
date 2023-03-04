#include <P18F4620.inc>
    config OSC = INTIO67
    config WDT = OFF
    config LVP = OFF
    config BOREN = OFF
				
    Delay1	equ	0xFF
    Delay2	equ	0xFF		; Constant declarations
    ORG	    0x0000
				; CODE STARTS FROM THE NEXT LINE uses array
START:
    ORG	    0x0000
START:
    MOVLW   0x0F		; Load W with 0x0F0
    MOVWF   ADCON1		; Make ADCON1 to be all digital
    MOVLW   0x00		; Load W with 0x00
    MOVWF   TRISA		; Make PORT A as outputs
    MOVLW   0x00		; Load W with 0x00
    MOVWF   TRISB		; Make PORT B as outputs
    MOVLW   0x00		; Load W with 0x00
    MOVWF   TRISC		; Make PORT C as outputs
    MOVLW   0x00		; Load W with 0x00
    MOVWF   TRISD		; Make PORT D as outputs
    
    MOVLW   0xC3		; Loads W with Hex color of D2 and D3 from previous lab
    MOVWF   0x30		; Moves W into register 30
    MOVLW   0xE1		; Loads W with Hex color of D2 and D3 from previous lab
    MOVWF   0x31		; Moves W into register 31
    MOVLW   0x47		; Loads W with Hex color of D2 and D3 from previous lab
    MOVWF   0x32		; Moves W into register 32
    MOVLW   0x62		; Loads W with Hex color of D2 and D3 from previous lab
    MOVWF   0x33		; Moves W into register 33
    MOVLW   0x86		; Loads W with Hex color of D2 and D3 from previous lab
    MOVWF   0x34		; Moves W into register 34
    MOVLW   0x20		; Loads W with Hex color of D2 and D3 from previous lab
    MOVWF   0x35		; Moves W into register 35
    MOVLW   0x04		; Loads W with Hex color of D2 and D3 from previous lab
    MOVWF   0x36		; Moves W into register 36
    MOVLW   0xA5		; Loads W with Hex color of D2 and D3 from previous lab
    MOVWF   0x37		; Moves W into register 37
 
MAIN_LOOP:			; start of While LOOP
    MOVLW   0x00		; load W with 0
    MOVWF   0x20		; store W to location 0x20
    MOVLW   0x08		; load W with 08
    MOVWF   0x21		; store W to location 0x21
 
    MOVLW   0x30		; load W with 0x30
    MOVWF   FSR0L		; store W to FSR0L
    MOVLW   0x00		; load W with 0x00
    MOVWF   FSR0H		; store W to FRS0H
 
FOR_LOOP:
    MOVF    0x20,W		; read content of 0x20 into W
    MOVWF   PORTC		; output W to PORT C
    MOVF    INDF0,W		; read content from array into W 
    MOVWF   PORTD		; output W to PORT D
    INCF    FSR0L,F		; increment array by 1
 
    CALL DELAY_ONE_SEC		; wait one sec
 
    INCF    0x20,F		; increment location 0x20 by 1
    DECF    0x21,F		; decrement location 0x21 by 1
    BNZ FOR_LOOP		; if not equal, then (0x21) not equal to 0
				; go back to FOR_LOOP
    GOTO MAIN_LOOP		; go back to While LOOP

DELAY_ONE_SEC:
    MOVLW Delay1		; Load constant Delay1 into W
    MOVWF 0x28			; Load W to memory 0x28
    
LOOP_1_OUTER:
    NOP				; Do nothing
    MOVLW Delay2		; Load constant Delay2 into W
    MOVWF 0x29			; Load W to memory 0x29
    
LOOP_1_INNER:
    NOP				; Do nothing
    DECF 0x29,F			; Decrement memory location 0x20
    BNZ LOOP_1_INNER		; If value not zero, go back to
    DECF 0x28,F			; Decrement memory location 0x28
    BNZ LOOP_1_OUTER		; If value not zero, go back to
    RETURN
END 






