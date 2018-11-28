
# TEST_RETURN_CODE = PASS

# allons au ru
.set noreorder
.text


    Lw $t0 , lunchtime
    LW $6, -200($7)
    ADDI $t1,$zero,8
boucle:
    BEQ $t0 , $t1 , byebye
    NOP
    addi $t1 , $t1 , 1
    J boucle 
    NOP
byebye:
    JAL vite(viteauru)

.data
lunchtime: 
    .word 12
.text
	adDi $t1 , $t1 , -0XEABCF
	addi $t1 , $t1 , 02546
	sw $t0 , lunchtime
	move $t3 ,$t2

.data
boucle:
    .word menu
    .asciiz "ils disent : \"au ru!\""
.bss 
menu:
    .space 24

