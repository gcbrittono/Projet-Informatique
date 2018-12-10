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

void genInstruction(inst_poly* bin,ListeG Inst, dico_bin tab[], int tailledico){
	int i=0;
	int k;
	int cas;
	while((i<tailledico) && (strcmp(((Instruction*)(Inst->pval))->nom,tab[i].instruction)!=0)){
	i+=1;
	}
    if(i==tailledico)
    	ERROR_MSG("erreur l'instruction n'est pas dans le dico");
	switch(((Instruction*)(Inst->pval))->type_inst){
		case 'R'  :
			cas=1;
			bin->r_inst.rd=00000;
			bin->r_inst.rt=00000;
			bin->r_inst.rs=00000;
			bin->r_inst.sa=00000;
			bin->r_inst.func =tab[i].code;
			/*for(k=0;k<tab[i].nbop;k++){
				if(strcmp(tab[i].type_op[k],"rd")==0)
					bin.r_inst.rd =((Instruction*)(Inst->pval))->op[k];
				else if (strcmp(tab[i]->type_op[k],"rt")==0)
					bin.r_inst.rt =((Instruction*)(Inst->pval))->op[k];
				else if (strcmp(tab[i]->type_op[k],"rs")==0)
					bin.r_inst.rs =((Instruction*)(Inst->pval))->op[k];
				else if (strcmp(tab[i]->type_op[k],"sa")==0)
					bin.r_inst.sa =((Instruction*)(Inst->pval))->op[k];
			}*/
			break;

		case 'I'  :
			cas=2;
			bin->i_inst.imm = 0000000000000000;
			bin->i_inst.rt =00000;
			bin->i_inst.rs =00000;
			bin->i_inst.opcode =tab[i].code;
			break;

		case 'J'  :
			cas=3;
			bin->j_inst.targ =00000000000000000000000000;
			bin->j_inst.opcode = tab[i].code;
			break;
	}
}

void gen(ListeG Inst, dico_bin tab[], int tailledico){
	inst_poly instr;
	unsigned int instruB;
	ListeG F=Inst->suiv;
		do{
			genInstruction(&instr, Inst, tab, tailledico);
			instruB = swap(instruB);
			/*printf("%x \n",instruB);*/
			printf("%d ",instr.r_inst.func);
			F=F->suiv;
		}while (F!=Inst->suiv);

}
