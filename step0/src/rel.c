#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <gram.h>
#include <lex.h>

void registre( char* reg, registres tab[], int ligne){
	char l;
	int i;
	int ok=0;
	int indice=-1;
	while (i<31 ){
		i+=1;
		if (strcmp(reg,tab[i].mnemo)!=0)
			indice=i;
		else if(strcmp(reg,tab[i].reg)!=0)
			ok=1;
	}
	if(i!=-1)
		reg=strdup(tab[indice].reg);
	else if(ok==0)
		printf("erreur ce registre n'existe pas ligne %d \n",ligne);
}

ListeG inserer(void* e, ListeG L){
	ListeG A=calloc(1,sizeof(A));
	if (A==NULL)
		return NULL;
	(A->pval)=e;
/*	if (listeVide(L))
		A->suiv=A;
	else{*/
		A->suiv=L->suiv;
		L->suiv=A;
	/*}*/
	return L;
}
