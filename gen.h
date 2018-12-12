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

unsigned int swap (unsigned int code);

void chargeDico(dico_bin tab[], int taille);

void gen(ListeG Inst, dico_bin tab[], int tailledico);

void genInstruction(inst_poly* bin, ListeG Inst, dico_bin tab[], int tailledico);

#endif
