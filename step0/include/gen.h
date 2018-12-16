#ifndef _GEN_H_
#define _GEN_H_


#include <stdio.h>
#include <lex.h>
#include <gram.h>

struct R{
	unsigned int func:6,
	sa:5,
	rd:5,
	rt:5,
	rs:5,
	opcode:6;
};

struct I{
	unsigned int imm:16,
	rt:5,
	rs:5,
	opcode:6;
};

struct J{
	unsigned int targ:26,
	opcode:6;
};

typedef union inst_poly {
	struct R r_inst ;
	struct I i_inst ;
	struct J j_inst ;
	char code[4];
} inst_poly;

typedef struct {
	char* instruction;
	char type;
	int nbop;
	char* type_op[3];
	int code;
} dico_bin;

unsigned int swap (unsigned int code);/*passe le binaire en big endian*/

void chargeDico(dico_bin tab[], int taille);/*charge le dictionnaire d'instruction qui contient le binaire*/

void gen(ListeG Inst, ListeG Data, dico_bin tab[], int tailledico, int tailletext, int tailledata, unsigned int texttab[], unsigned int datatab[]);/*fonction qui réalise la génération du binaire pour text et data (incomplète)*/

int genInstruction(inst_poly* bin,ListeG Inst, dico_bin tab[], int tailledico);/*génère le bianire pour les instruction*/

void gendata(/*unsigned int* binairedata,*/ListeG Data, unsigned int datatab[]);/*fonction pour générer le binaire du code data en fonction de la directive et le stocke dans le tablau datatab*/

#endif
