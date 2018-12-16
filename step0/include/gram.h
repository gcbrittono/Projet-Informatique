#ifndef _GRAM_H_
#define _GRAM_H_


#include <stdio.h>
#include "lex.h"



/*typedef enum {INIT, COMMENTAIRE, SYMBOLE, DIRECTIVE, REGISTRE,  DEUX_POINTS, VIRGULE, SAUT_DE_LIGNE, HEXA_DEBUT, OCTATE, DECIMAL_ZEROS, DECIMAL, TERM, HEXA, CITATION,PARENTHESE} etat;*/

/*Définition de macro etats*/
typedef enum {DATA, BSS, TEXT, UNDEFINED} Section;

/*Definition de structure pour charger les donees de l'instruction dans un dictionnaire, il y a les donnes de symbole de l'instruction, type soit r,i ou j et le nombre des operands*/
typedef struct DictionnaireIn{ 
	char* symbole;
    	char type;
    	int operands;
	int col;
	char* type_op[3];
} Dico;

/*Liste circulaire Générique*/
struct el{
	void* pval;
	struct el* suiv;
};
typedef struct el * ListeG;

typedef struct {
	char* lexeme;
	etat categorie;
	char* typeadr;
} Operande;

typedef struct {
	char* nom;
	etat type;
	char type_inst;
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
	Opedonnee valeur;
	etat type;
} OpeD;

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

ListeG creerListeG();/*cré une liste générique G (en réalité file qui pointe sur le dernier élément*/

int listeVide(ListeG L);/*teste si la liste générique est vide*/

Instruction* creerInstruction(char* lex, etat cat,int nombop, int lig, unsigned int dec, char type/*, ListeG operande*/);/*crée la structure de l'instruction à partir de son contenue, sans remplir le contenue des opérandes*/

Donnee1* creerDonnee1(char* lex, etat cat, int nombop, int lig, int dec, ListeG operande);/*pour créer la structure pour les data*/

Donnee2* creerDonnee2(char* lex, etat cat, int nombop, int lig, int dec, int val);/*pour créer la structure pour la section bss*/

Symbole* creerSymbole(char* lex, etat cat, int lig, Section section ,int dec);/*pour créer la structure pour la section symbole*/

ListeG ajouterQueue(void* e, ListeG L);/*fonction qui ajoute un élément générique à la liste (file)*/

void afficherInst(Instruction* L);/*fonction d'affichage des instruction*/

void afficherDo1(Donnee1* L);/*fonction d'affichage des data*/

void afficherDo2(Donnee2* L);/*fonction d'affichage des bss*/

void afficherSymb(Symbole* L);/*fonctino d'affichage des symboles*/

int funHash(char* str, int taille);/*fonction de hachage (n'est finalement pas utilisée)*/

void toLowerStr(char *str);/*fonction qui permet de passer du texte en minuscule, sert pour la comparaison des instructions*/

void machine_a_etat_gram (File F, ListeG* Inst, ListeG* Symb, ListeG* Do1, ListeG* Do2, Dico tableau[], int taille, int* erreur);/*machine à état de l'annalyse grammaticale*/

void gramAnalyse(File F, ListeG* Inst, ListeG* Symb, ListeG* Do1, ListeG* Do2, int* erreur);/*fonction qui fait l'annalyse grammaticale de code*/ 

/*File defiler*/

#endif 

