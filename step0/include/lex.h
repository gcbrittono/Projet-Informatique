/**
 * @file lex.h
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexem-related stuff.
 *
 * Contains lexem types definitions, some low-level syntax error codes,
 * the lexem structure definition and the associated prototypes.
 */

#ifndef _LEX_H_
#define _LEX_H_


#include <stdio.h>

typedef enum {INIT, COMMENTAIRE, SYMBOLE, DIRECTIVE, REGISTRE,  DEUX_POINTS, VIRGULE, SAUT_DE_LIGNE, HEXA_DEBUT, OCTATE, DECIMAL_ZEROS, DECIMAL, TERM, HEXA, CITATION,PARENTHESE,SIGNE,BASE_OF, BASE} etat;

typedef struct lex {
	char* lexeme;
	etat categorie;
	int ligne;
	struct lex * suiv;
} lex_t;

typedef lex_t *File;

/*fonction qui crée une liste*/
File creerFile();

/*fonction qui vérifie si la liste est vide*/
int estVide(File L);

/*fonction qui créun élément contenant le lexeme lex, de type cat et qui est sur la ligne lig*/

lex_t creerElement(char* lex, etat cat, int lig);

/*fonction qui enfile l'element L a la file F*/
File enfiler(lex_t L, File F);

/*fonction qui affiche le contenue de la file :  le lexeme, sa ligne et sa catégorie*/
void afficherFile(File F);

void	lex_read_line( char *, int , File *F);
void	lex_load_file( char *, unsigned int *, File *F );
char* 	getNextToken( char** , char* );

#endif /* _LEX_H_ */

