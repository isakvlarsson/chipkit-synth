.text
# Mips code for global interrupt enabling
.global enable_interrupt
enable_interrupt:
	ei
	jr $ra 
	nop 