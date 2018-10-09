#ifndef _FONCTION_LISTE_H_
#define _FONCTION_LISTE_H_

#include <stdio.h>

typedef struct lex {
	char* lexeme;
	int categorie;
	int ligne;
	struct lex * suiv;
} lex_t;

typedef lex_t *File;

//fonction qui crée une liste
File creerFile();

//fonction qui vérifie si la liste est vide
int estVide(File L);

//fonction qui créun élément contenant le lexeme lex, de type cat et qui est sur la ligne lig

lex_t creerElement(char* lex, int cat, int lig);

//fonction qui enfile l'element L a la file F
File enfiler(lex_t L, File F);

//fonction qui affiche le contenue de la file :  le lexeme, sa ligne et sa catégorie
void afficherFile(File F);

#endif
