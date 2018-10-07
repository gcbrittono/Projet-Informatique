#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Fonction_Liste.h"

//fonction qui crée une liste
File creerFile(){
	return NULL;
}

//fonction qui vérifie si la liste est vide
int estVide(File L){
	return !L;
}

//fonction qui ajoute un maillon à la fin de la file contenant le lexeme lex, de type cat et qui est sur la ligne lig, à la liste L
lex_t creerElement(char* lex, int cat, int lig){
	lex_t p;
	strcpy(p.lexeme,lex);
	p.categorie=cat;
	p.ligne=lig;
	return p;
}

File enfiler(lex_t L, File F){
	File A=calloc(1,sizeof(*A));
	if (A==NULL)
		return NULL;
	A->element=L;
	if (estVide(F))
		A->suiv=A;
	else{
		A->suiv=F->suiv;
		F->suiv=A;
	}
	return A;
}
	
	



//afiche le premier élément de la file
void afficherFile(File F){
	if (estVide(F)) /*cas ou la file est vide, on sort de la fonction*/
		printf("la file est vide");
		return;
	printf("Ligne %d : [ %s ] : %s \n", F->suiv->element.ligne, F->suiv->element.categorie, F->suiv->element.lexeme);
}

//retire le premier élément de la file 
//File defiler(F)
