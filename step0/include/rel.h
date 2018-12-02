#ifndef _REL_H_
#define _REL_H_


#include <stdio.h>
#include <lex.h>
#include <gram.h>

typedef struct {
char * mnemo;
char * reg;
} registres;


void registre( char* reg, registres tab[32], int ligne);

ListeG inserer(void* e, ListeG L);

void libererInstruction(ListeG* L);

void associerReg(ListeG Inst,registres tableau[32],int ligne);

void chargeRegistre(registres tab[32]);

void relocationInst(ListeG Inst,ListeG* Symb,int ligne,ListeG* RelocInst);

Symbole* trouverSymbole(char* nom, int ligne, ListeG* Symb);

void rel(ListeG* Instruct, ListeG Data, ListeG* Etiquette, ListeG* RelocInst, ListeG* RelocData);

/*-----------------------relocation--------------------*/

typedef enum {R_MIPS_32=2, R_MIPS_26=4, R_MIPS_HI16=5,
R_MIPS_LO16=6} mode_rel;

typedef struct table_relocation {
	char* nom_section;
	unsigned int addr_relative;
	mode_rel mode_relocation;
	Symbole* pointeur; 
}table_relocation;

table_relocation*  symbole_find(ListeG L, Symbole* symb, int i /*entier qui indique si data ou text*/);

void remplacer_instr(ListeG listeInstr, char* nom_instr_final, int nombop_instr_final, char type_instr_final, char* ope[], etat typop[]);

void pseudoInstruction( ListeG* instr);

#endif 
