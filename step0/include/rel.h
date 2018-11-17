#ifndef _REL_H_
#define _REL_H_


#include <stdio.h>

typedef enum {R_MIPS_32=2, R_MIPS_26=4, R_MIPS_HI16=5,
R_MIPS_LO16=6} mode_rel;

typedef struct table_relocation {
	char* nom_section;
	unsigned int addr_relative;
    mode_rel mode_relocation;
    symbol* pointeur; 
};













#endif
