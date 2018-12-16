#ifndef _REL_H_
#define _REL_H_


#include <stdio.h>
#include <lex.h>
#include <gram.h>

typedef struct {
char * mnemo;
char * reg;
} registres;


void registre( char* reg, registres tab[32], int ligne);/*fonction qui vérifie le registre reg et passe les mnémonique en chiffre en comparant à un dictionnaire de registre*/

ListeG inserer(void* e, ListeG L);/*fonction qui insère une instruction ddans la liste (file) générique a la position pointé par L*/

void associerReg(ListeG Inst,registres tableau[32],int ligne);/*fonction sui vérifie et remplace toutes les mnémonique des registres de la liste d'instruction*/

void chargeRegistre(registres tab[32]);/*fonction qui charge le dictionnaire de registre*/

void relocationInst(ListeG Inst,ListeG* Symb,int ligne,ListeG* RelocInst);/*fonction pour faire la relocation totale d'une instruction si nécessaire*/

void relocationData(ListeG Data,ListeG* Symb,int ligne,ListeG* RelocData);/*fonction qui fait la relocation de la section data*/

Symbole* trouverSymbole(char* nom, int ligne, ListeG* Symb);/*fonction qui trouve un symbole par som nom dans la table des symboles, et le rajoute à la fin avec la mention undefined s'il n'existe pas encore*/

void rel(ListeG* Instruct, ListeG Data, ListeG* Etiquette, ListeG* RelocInst, ListeG* RelocData);/*fonction qui réalise la relocation des sections text et data en remplissant les liste RelocInst et RelocDta*/

/*-----------------------relocation--------------------*/

typedef enum {R_MIPS_32=2, R_MIPS_26=4, R_MIPS_HI16=5,
R_MIPS_LO16=6} mode_rel;

typedef struct table_relocation {/*structure pour la table de relocation*/
	/*char* nom_section;*/
	Section sect;
	unsigned int addr_relative;
	mode_rel mode_relocation;
	Symbole* pointeur; 
}table_relocation;

table_relocation*  symbole_find(ListeG L, Symbole* symb, int i /*entier qui indique si data ou text*/);

void remplacer_instr(ListeG listeInstr, char* nom_instr_final, int nombop_instr_final, char type_instr_final, char* ope[], etat typop[]);/*fonction qui remplace l'instruction pointée par listeInstr par une autre dont le contenue est donné (sans modifier les opérandes)*/

void pseudoInstruction( ListeG* instr);/*fonction qui remplace les pseudo instruction par les instructions auquelles elles correspondent*/

Symbole* trouverSymbole2(char* nom, int ligne, ListeG* Symb);/*fonction qui retrouve un symbole dans la table des symbole à partir de son nom*/

void extraction_des_operandes(Instruction* inst, ListeG Symb);/*fonction qui vérifie la validité des opérandes de l'instruction inst en type et en taille*/

/*fonctions de libération de la mémoire des différentes structures créés*/

void libererInstruction(ListeG* L);

void libererDo1(ListeG* L);

void libererDo2(ListeG* L);

void libererSymbole(ListeG* L);

void libererFile(File* L);

void libererregistre(registres tab[], int taille);

void libererdico(Dico d[], int taille);

#endif 
