#ifndef _REL_H_
#define _REL_H_


#include <stdio.h>
#include "lex.h"
#include "gram.h"



typedef enum {R_MIPS_32=2, R_MIPS_26=4, R_MIPS_HI16=5,
R_MIPS_LO16=6} mode_rel;

typedef struct table_relocation {
	char* nom_section;
	unsigned int addr_relative;
    mode_rel mode_relocation;
    struct table_relocation* pointeur; 
}table_relocation;




/*Fonction pour trouver un symbole et l'ajouté au tableu de relocation*/
table_relocation  symbole_find(Symbole* symb);

/*Fonction pour trouver un instruction et l'ajouté au tableu de relocation*/
table_relocation  instruction_find(Instruction* inst);

ListeG* trouver_pseudoinstruction(/*Instruction* inst*/ File listeInstr,Dico tableau[],int dec_text, int position);






#endif
