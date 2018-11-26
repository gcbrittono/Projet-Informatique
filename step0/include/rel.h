#ifndef _REL_H_
#define _REL_H_


#include <stdio.h>
<<<<<<< HEAD
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
=======
#include <lex.h>
#include <gram.h>

typedef struct {
char * mnemo;
char * reg;
} registres;


void registre( char* reg, registres tab[32], int ligne);

ListeG inserer(void* e, ListeG L);

void associerReg(ListeG Inst,registres tableau[32],int ligne);

void chargeRegistre(registres tab[32]);

void rel(ListeG Instruct);

#endif 
>>>>>>> bbe9c41c8b1e9ad56cc24e8a67a13b4a5a51970c
