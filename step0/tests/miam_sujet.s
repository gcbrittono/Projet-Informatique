
# TEST_RETURN_CODE = PASS

# allons au ru
.set noreorder
.text


    Lw $t0 , lunchtime
    LW $6, -200($t3)
    ADDI $t1,$zero,8
boucle:
    BEQ $t0 , $t1 , byebye
    NOP
    addi $t1 , $t1 , 1
    J boucle 
    NOP
byebye:
    JAL vite

.data
lunchtime: 
    .word 12
.text
	adDi $t1 , $t1 , -0XEABCF
	addi $t1 , $t1 , 02546
	sw $t0 , lunchtime
	sw $t0 , bonjour
	move $t3 ,$t2

.data
boucles:
    .word menu, 12
    .asciiz "ils disent : \"au ru!\""
.bss 
menu:
    .space 24

