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


unsigned int swap (unsigned int code){/*fonction pour mettre le mot en big endian*/
	code = ((code >> 24)& 0x000000FF) | ((code << 24)&
0xFF000000) | ((code >> 8)& 0x0000FF00) | ((code << 8)&
0x00FF0000);
	return code;
}

void chargeDico(dico_bin tab[], int taille){/*fonction pour charger le deuxième dictionnaire d'instruction*/
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

void gendata(/*unsigned int* binairedata,*/ListeG Data, unsigned int datatab[]){/*fonction pour générer le binaire du code data en fonction de la directive et le stocke dans le tablau datatab*/
	int k;
	ListeG B=Data->suiv;
	ListeG C=Data;
	FILE *data = fopen("data.txt","w+t");
	if(data==NULL)
		ERROR_MSG("echec de création du fichier data");
	do{
		ListeG o=((Donnee1*)(B->pval))->op;
		Opedonnee d;
		int i;
		if(strcmp(((Donnee1*)(B->pval))->lexeme, ".byte")==0){
			for(i=0;i<((Donnee1*)(B->pval))->nbop;i++){
				d=((OpeD*)(o->pval))->valeur;
				/*datatab[k+i]=d.byte;*/
				fprintf(data,"%d",d.byte);
				o=o->suiv;
			}
		}
		else if(strcmp(((Donnee1*)(B->pval))->lexeme, ".asciiz")==0){
			for(i=0;i<((Donnee1*)(B->pval))->nbop;i++){
				int j;
				for(j=0;j<strlen(((OpeD*)(o->pval))->valeur.as_et);j++){
					/*datatab[k+i+j]=((Donnee1*)(B->pval))->lexeme[j];*/
					fprintf(data,"%d",((OpeD*)(o->pval))->valeur.as_et[j]);
				}
				o=o->suiv;
			}
		}
		else if(strcmp(((Donnee1*)(B->pval))->lexeme, ".word")==0){
			for(i=0;i<((Donnee1*)(B->pval))->nbop;i++){
				d=((OpeD*)(o->pval))->valeur;
				/*datatab[k+i]=d.word;*/
				int dec=((((Donnee1*)(B->pval))->decalage)%4);
				if(dec!=0){
					int q;
					for(q=0;q<(4-((((Donnee1*)(B->pval))->decalage)%4));q++)
						fprintf(data,"%c",'0');
				}
				fprintf(data,"%u",swap(d.word));
				o=o->suiv;
			}
		}
		else if(strcmp(((Donnee1*)(B->pval))->lexeme, ".space")==0){
			for(i=0;i<((Donnee1*)(B->pval))->nbop;i++){
				d=((OpeD*)(o->pval))->valeur;
				int n;
				for(n=0;n<strtol(d.word,NULL,0);n++)
					fprintf(data,"%c",'0');			
				o=o->suiv;
			}
		}
			
		B=B->suiv;
		C=C->suiv;
		k+=1;
	}while (B!=Data->suiv);
		
	fclose(data);
}

int genInstruction(inst_poly* bin,ListeG Inst, dico_bin tab[], int tailledico){/*fonction pour générer le code binaire des instructions*/
	int i=-1;
	int k;
	int cas;
	printf("\ninstruction %s\n",((Instruction*)(Inst->pval))->nom);
	do{
			i+=1;
	}while((i<tailledico) && (strcmp(((Instruction*)(Inst->pval))->nom,tab[i].instruction)!=0));/*pour trouver l'indice de l'instruction dans le dico*/
    	if(i==tailledico)
    		ERROR_MSG("erreur l'instruction n'est pas dans le dico");/*erreur si l'instruction n'est pas dans le dici, normalement déja vérifié*/
	switch(tab[i].type){/*switch qui génère le code binaire en fonction du type R, I ou J de l'instruction*/
		case 'R'  :
			cas=1;
			/*initialisation des opérandes et remplissage de l'opcode ou de func*/
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
				else if (strcmp(tab[i].type_op[k],"sa")==0)
					bin->r_inst.sa =strtol(((Instruction*)(Inst->pval))->op[k].lexeme,NULL,0);
				if (strcmp(((Instruction*)(Inst->pval))->nom,"rotr")==0)
					bin->r_inst.rs=00001;
			}
			break;

		case 'I'  :
			cas=2;
			/*initialisation des opérandes et remplissage de l'opcode ou de func*/
			bin->i_inst.imm = 0000000000000000;
			bin->i_inst.rt =00000;
			bin->i_inst.rs =00000;
			bin->i_inst.opcode =tab[i].code;
		/*	if(strcmp(((Instruction*)(Inst->pval))->nom,"lw")==0 || strcmp(((Instruction*)(Inst->pval))->nom,"sw")==0){
				for(k=0;k<tab[i].nbop;k++){
					if (strcmp(tab[i].type_op[k],"rt")==0)
						bin->i_inst.rt =atoi(((Instruction*)(Inst->pval))->op[k].lexeme+1);
					else if(strcmp(tab[i].type_op[k],"ofrs")==0){
						char* token = ((Instruction*)(Inst->pval))->op[i].lexeme;
						int j=0;
						while(*token!='('){
							printf("bien");
							token++;
							j+=1;
						}
						token++;
						int k=0;
						while(token[k]!=')')
							k+=1;
						char* reg;
						char* offset;
						reg = strndup(token,k);
						offset = strndup(((Instruction*)(Inst->pval))->op[i].lexeme,j);
						bin->i_inst.rs =atoi(reg+1);
						bin->i_inst.imm =strtol(offset,NULL,0);
					}
				}
			}
			else{*/
				for(k=0;k<tab[i].nbop;k++){/*ne fonctionne pas pour les I base offset*/
					if (strcmp(tab[i].type_op[k],"rt")==0)
						bin->i_inst.rt =atoi(((Instruction*)(Inst->pval))->op[k].lexeme+1);
					else if (strcmp(tab[i].type_op[k],"rs")==0)
						bin->i_inst.rs =atoi(((Instruction*)(Inst->pval))->op[k].lexeme+1);
					else if (strcmp(tab[i].type_op[k],"im")==0)
						bin->i_inst.imm =strtol(((Instruction*)(Inst->pval))->op[k].lexeme,NULL,0);

				}
		/*	}*/
			break;

		case 'J'  :
			cas=3;
			/*initialisation des opérandes et remplissage de l'opcode ou de func*/
			bin->j_inst.targ =00000000000000000000000000;
			bin->j_inst.opcode = tab[i].code;
			bin->j_inst.targ =strtol(((Instruction*)(Inst->pval))->op[0].lexeme,NULL,0);
			break;
	}
	return cas;
}

void gen(ListeG Inst, ListeG Data, dico_bin tab[], int tailledico, int tailletext, int tailledata,unsigned int texttab[], unsigned int datatab[]){/*fonction qui génère le code binaire pour data et text (incomplète)*/
	inst_poly instr;
	unsigned int instruB;/*contient le code binaire des instruction*/
	int m=0;/*indice de texttab à remplir*/
	if(!listeVide(Inst)){
	ListeG F=Inst->suiv;
	int cas;
		do{
			cas = genInstruction(&instr, F, tab, tailledico);
			/*affichage du code des instruction en hexadécimal*/
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
			/*remplissage du tbleau d'instruction avec le binaire*/
			texttab[m]=instruB;
			m+=1;
			F=F->suiv;
		}while (F!=Inst->suiv);
	}
	
	/*gendata(Data, datatab);*/

}
