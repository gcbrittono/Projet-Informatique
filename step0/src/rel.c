#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <gram.h>
#include <lex.h>
#include <rel.h>

void registre( char* reg, registres tab[32], int ligne){
	int i=0;;
	int ok=0;
	int indice=-1;
	while (i<32 ){
		if (strcmp(reg,tab[i].mnemo)==0)
			indice=i;
		else if(strcmp(reg,tab[i].reg)==0)
			ok=1;
		i+=1;
	}
	if(indice!=-1)
		strcpy(reg,tab[indice].reg);
	else if(ok==0)
		printf("erreur ce registre n'existe pas ligne %d \n",ligne);/*gestion erreurs arreter compil?*/
}

ListeG inserer(void* e, ListeG L){
	ListeG A=calloc(1,sizeof(A));
	if (A==NULL)
		return NULL;
	(A->pval)=e;
	if (listeVide(L)){
		A->suiv=A;
		return A;
	}
	else{
		A->suiv=L->suiv;
		L->suiv=A;
	}
	return L;
}

void associerReg(ListeG Inst,registres tableau[32],int ligne){
	int nb=((Instruction*)(Inst->pval))->nbop;
	int i = 0;
	for(i; i<nb;i++){
		if(((Instruction*)(Inst->pval))->op[i].categorie==REGISTRE)
			registre(((Instruction*)(Inst->pval))->op[i].lexeme,tableau,ligne);
	}
}

void chargeRegistre(registres tab[32]){
FILE* re;

	re = fopen("src/registre.txt","r");
	if (re == NULL){
		printf("le dictionnaire de registre n'a pas été ouvert \n"); 
		return; /*gestion erreurs*/
	}
	char* reg1=malloc(sizeof(*reg1));
	char* reg2=malloc(sizeof(*reg2));
	int i;
	for (i=0;i<32;i++){
        	fscanf(re, "%s %s",reg1,reg2);
		tab[i].mnemo=strdup(reg1);
		tab[i].reg=strdup(reg2);
	}
	fclose(re);
}

void rel(ListeG Instruct){
	registres tab[32];
	chargeRegistre(tab);
	if(listeVide(Instruct))
		printf("la section TEXT est vide");
	else{
		ListeG A=Instruct;
		do{
			associerReg(A->suiv,tab,((Instruction*)(A->suiv->pval))->ligne);






			A=A->suiv;
		}while (A!=Instruct);
	}
}


















