#ifndef _GRAM_H_
#define _GRAM_H_


#include <stdio.h>
#include "lex.h"



/*typedef enum {INIT, COMMENTAIRE, SYMBOLE, DIRECTIVE, REGISTRE,  DEUX_POINTS, VIRGULE, SAUT_DE_LIGNE, HEXA_DEBUT, OCTATE, DECIMAL_ZEROS, DECIMAL, TERM, HEXA, CITATION,PARENTHESE} etat;*/

/*Définition de macro etats*/
typedef enum {DATA, BSS, TEXT} Section;

/*Definition des types des operans pour le dictionnaire*/
typedef enum {Reg, Imm, sa, Bas, Rel, Abs} typ_op;

/*Definition de la fonction pour faire l'analyse des pseudoinstuctions*/

Dico* pseudoInstruction(Dico* p);


/*Definition de structure pour charger les donees de l'instruction dans un dictionnaire, il y a les donnes de symbole de l'instruction, type soit r,i ou j et le nombre des operands*/
typedef struct DictionnaireIn{ 
	char* symbole;
    	char type;
    	int operands;
	int col;
	typ_op typeOperande[2]; 
} Dico;

/*Liste circulaire Générique*/
typedef struct el{
	void* pval;
	struct el* suiv;
} *ListeG;

typedef struct {
	char* lexeme;
	etat categorie;
} Operande;

typedef struct {
	char* nom;
	etat type;
	int nbop;
	int ligne;
	unsigned int decalage;
	Operande op[3];
} Instruction;

typedef union {
	char byte;
	int word;
	char* as_et;
	unsigned int space;
	unsigned char octet;
} Opedonnee;
	
	

typedef struct {
	char* lexeme;
	etat type;
	int nbop;
	int ligne;
	int decalage;
	ListeG op;
} Donnee1; /*données data*/
	

typedef struct {
	char* lexeme;
	etat type;
	int nbop;
	int ligne;
	int decalage;
	int valeur;
} Donnee2;/*données bss*/

typedef struct {
	char* lexeme;
	etat type;
	int ligne;
	Section sect;
	int decalage;
} Symbole;


/*typedef union {
	Donnee1 don1;
	Donnee2 don2;
	Instruction inst;
	Symbole symb;
} Element;*/

ListeG creerListeG();

int listeVide(ListeG L);

Instruction* creerInstruction(char* lex, etat cat,int nombop, int lig, unsigned int dec/*, ListeG operande*/);

Donnee1* creerDonnee1(char* lex, etat cat, int nombop, int lig, int dec, ListeG operande);

Donnee2* creerDonnee2(char* lex, etat cat, int nombop, int lig, int dec, int val);

Symbole* creerSymbole(char* lex, etat cat, int lig, Section section ,int dec);

ListeG ajouterQueue(void* e, ListeG L);

void afficherInst(Instruction* L);

void afficherDo1(Donnee1* L);

void afficherDo2(Donnee2* L);

void afficherSymb(Symbole* L);

int funHash(char* str, int taille);

void toLowerStr(char *str);

void machine_a_etat_gram (File F, ListeG* Inst, ListeG* Symb, ListeG* Do1, ListeG* Do2, Dico tableau[], int taille);

void gramAnalyse(File F, ListeG* Inst, ListeG* Symb, ListeG* Do1, ListeG* Do2);

/*File defiler*/

#endif 

