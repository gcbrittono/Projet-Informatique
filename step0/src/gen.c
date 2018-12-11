#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <gram.h>
#include <lex.h>
#include <rel.h>
#include <gen.h>
#include <global.h>
#include <notify.h>


unsigned int swap (unsigned int code){
	code = ((code >> 24)& 0x000000FF) | ((code << 24)&
0xFF000000) | ((code >> 8)& 0x0000FF00) | ((code << 8)&
0x00FF0000);
	return code;
}

void chargeDico(dico_bin tab[], int taille){
FILE* inst;
	inst = fopen("src/dico.txt","r");
	if (inst == NULL){
		ERROR_MSG("le dictionnaire d'instruction n'a pas été ouvert ");
		return;
	}
	char instru[10];
	char type;
	int nbop;
	char reg1[4];
	long op;
	int i;
	int j;
	for (i=0;i<taille;i++){
        	fscanf(inst, "%s %c %d ",instru,&type,&nbop);
		tab[i].instruction=strdup(instru);
		tab[i].type=type;
		tab[i].nbop=nbop;
		for(j=0;j<nbop;j++){
			fscanf(inst, "%s ",reg1);
			tab[i].type_op[j]=strdup(reg1);
		}
		fscanf(inst, "%ld",&op);
		tab[i].code=op;
	}
	fclose(inst);

}

int genInstruction(inst_poly* bin,ListeG Inst, dico_bin tab[], int tailledico){
	int i=-1;
	int k;
	int cas;
	printf("\ninstruction %s\n",((Instruction*)(Inst->pval))->nom);
	do{
			i+=1;
	}while((i<tailledico) && (strcmp(((Instruction*)(Inst->pval))->nom,tab[i].instruction)!=0));
    if(i==tailledico)
    	ERROR_MSG("erreur l'instruction n'est pas dans le dico");
	switch(tab[i].type){
		case 'R'  :
			cas=1;
			bin->r_inst.rd=00000;
			bin->r_inst.rt=00000;
			bin->r_inst.rs=00000;
			bin->r_inst.sa=00000;
			bin->r_inst.func =tab[i].code;
			for(k=0;k<tab[i].nbop;k++){
				if(strcmp(tab[i].type_op[k],"rd")==0){
					bin->r_inst.rd =atoi(((Instruction*)(Inst->pval))->op[k].lexeme+1);
				}
				else if (strcmp(tab[i].type_op[k],"rt")==0)
					bin->r_inst.rt =atoi(((Instruction*)(Inst->pval))->op[k].lexeme+1);
				else if (strcmp(tab[i].type_op[k],"rs")==0)
					bin->r_inst.rs =atoi(((Instruction*)(Inst->pval))->op[k].lexeme+1);
				/*else if (strcmp(tab[i].type_op[k],"sa")==0)
					bin->r_inst.sa =atoi(((Instruction*)(Inst->pval))->op[k].lexeme); //avérifier*/
			}
			break;

		case 'I'  :
			cas=2;
			bin->i_inst.imm = 0000000000000000;
			bin->i_inst.rt =00000;
			bin->i_inst.rs =00000;
			bin->i_inst.opcode =tab[i].code;
			for(k=0;k<tab[i].nbop;k++){
				if (strcmp(tab[i].type_op[k],"rt")==0)
					bin->i_inst.rt =atoi(((Instruction*)(Inst->pval))->op[k].lexeme+1);
				else if (strcmp(tab[i].type_op[k],"rs")==0)
					bin->i_inst.rs =atoi(((Instruction*)(Inst->pval))->op[k].lexeme+1); /*attention les bases offset ne sont pas pris en comptes*/
				/*else if (strcmp(tab[i].type_op[k],"im")==0)
					bin->i_inst.imm =((Instruction*)(Inst->pval))->op[k].lexeme;*/
			}
			break;

		case 'J'  :
			cas=3;
			bin->j_inst.targ =00000000000000000000000000;
			bin->j_inst.opcode = tab[i].code;
			/*
					bin->j_inst.targ =((Instruction*)(Inst->pval))->op[0].lexeme;
			}*/
			break;
	}
	return cas;
}

void gen(ListeG Inst, dico_bin tab[], int tailledico){
	inst_poly instr;
	unsigned int instruB;
	if(!listeVide(Inst)){
	ListeG F=Inst->suiv;
	int cas;
		do{
			cas = genInstruction(&instr, F, tab, tailledico);
			if(cas==1){
				printf("instruction R\n");

				instruB = ((0xFC000000 & (instr.r_inst.opcode << 26)) |((instr.r_inst.rs << 21)& 0x03E00000) | ((instr.r_inst.rt << 16)& 0x001F0000) | ((instr.r_inst.rd << 11)& 0x0000F800)| ((instr.r_inst.sa << 6)& 0x000007C0) | ((instr.r_inst.func) & 0x0000003F));
				instruB = swap(instruB);
			}
			else if(cas==2){
				printf("instruction I\n");

				instruB = ((0xFC000000 & (instr.i_inst.opcode << 26)) |((instr.i_inst.rs << 21)& 0x03E00000) | ((instr.i_inst.rt << 16)& 0x001F0000) | ((instr.i_inst.imm)& 0x0000FFFF));
				instruB = swap(instruB);
			}
			else if(cas==3){
				printf("instruction J\n");

				instruB = ((0xFC000000 & (instr.j_inst.opcode << 26)) |((instr.j_inst.targ << 21)& 0x03FFFFFF) );
				instruB = swap(instruB);
			}
			printf("%x \n",instruB);
			F=F->suiv;
		}while (F!=Inst->suiv);
	}

}
