#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Fonction_Liste.h"



main(){
enum {INIT, COMMENTAIRE, SYMBOLE, DIRECTIVE, REGISTRE,  DEUX_POINTS, VIRGULE, SAUT_DE_LIGNE, HEXA_DEBUT, OCTATE, DECIMAL_ZEROS, DECIMAL, TERM, HEXA, CITATION};
File F;
char* lexeme1;
int ligne1=9;
int S1= HEXA;

char* lexeme2[10];
int ligne2=15;
int S2=REGISTRE ;



F=creerFile();

/*test de la fonction estvide pour une liste vide*/
if (estVide(F)){
	printf("1 OK \n");
}
else{
	printf("1/ NOK ");
}

lexeme1="bonjour";

F=enfiler(creerElement(lexeme1, 9,ligne1),F);

/*test de la fonction estvide pour une liste vide*/
if (estVide(F))
	printf("2/ NOK \n");
else
	printf("2/ OK \n");




/*test fonction enfiler et de son affichage lorsque la file ne contient qu'un élément*/

afficherFile(F);

printf("o");
/*
*lexeme2='$25';

F=enfiler(lexeme2, S2,ligne2,F);
*/
/*test fonction enfiler et de son affichage lorsque la file contient plusieurs éléments*/
/*
afficherFile(F);

if (F->ligne==9)
	printf("3/ OK \n");
else
	printf("3/ NOK \n");

*/







}












