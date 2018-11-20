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

void associerReg(ListeG Inst,registres tableau[32],int ligne);

void chargeRegistre(registres tab[32]);

void rel(ListeG Instruct);

#endif 
