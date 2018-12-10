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

unsigned int genInstruction(ListeG Inst, dico_bin tab[], int tailledico){
	int i;
	while((i<tailledico) && (strcmp(((Instruction*)(Inst->pval))->nom,tab[i].instruction)!=0)){
	i++
	}
    if(i==tailledico)
    	ERROR_MSG("erreur l'instruction n'est pas dans le dico");
    inst_poly bin;
	switch(((Instruction*)(Inst->pval))->type_inst){
		case 'R'  :
      			/*struct R bin ;*/
			bin.r_inst.func =
			bin.r_inst.sa =
			bin.r_inst.rd =
			bin.r_inst.rt =
			bin.r_inst.rs =
			bin.r_inst.opcode =
			break; 

		case 'I'  :
      			/*struct I bin ;*/
			bin.i_inst.imm = 
			bin.i_inst.rt =
			bin.i_inst.rs =
			bin.i_inst.opcode =			
			break; 

		case 'J'  :
      			/*struct I bin ;*/
			bin.j_inst.targ = 
			bin.j_inst.opcode =    				
			break;
	}
	return bin;

void chargeDico(dico_bin tab[], int taille){
FILE* inst;
	inst = fopen("src/dico.txt","r");
	if (inst == NULL){
		ERROR_MSG("le dictionnaire de registre n'a pas été ouvert "); 
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



