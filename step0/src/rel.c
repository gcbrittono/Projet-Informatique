#include <rel.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>



Symbole* creerSymbole(char* lex,	etat cat, int lig, Section section ,int dec){
	Symbole* p=malloc(sizeof(*p));
	p->lexeme=strdup(lex);
	p->type=cat;
	p->ligne=lig;
	p->decalage=dec;
	p->sect=section;
	return p;
}

typedef struct {
	char* nom;
	etat type;
    char typ_inst;
	int nbop;
	int ligne;
	unsigned int decalage;
	Operande op[3];
} Instruction;



/*typedef enum {R_MIPS_32=2, R_MIPS_26=4, R_MIPS_HI16=5,
R_MIPS_LO16=6} mode_rel;*/

/*Fonction pour trouver un symbole*/
void symbole_find(Symbole* symb){
    
    table_relocation tab_rel;

        if(strcmp(symb->sect, ".data")==0){
            tab_rel.nom_section = "data";
            tab_rel.addr_relative = symb->decalage;
            tab_rel.mode_relocation = 2;
            tab_rel.pointeur = symb;
				
		}
        else if(strcmp(symb->sect,".text")==0){
            tab_rel.nom_section = "text";
            tab_rel.addr_relative = symb->decalage;
            tab_rel.mode_relocation = 2;  /*Je ne suis pas sur si ca marcherá dans le cas text*/
			tab_rel.pointeur = symb;	
		}

    
}

/*Fonction pour trouver un instruction*/
void instruction_find(Instruction* inst){
    
    table_relocation tab_rel;

    tab_rel.nom_section = "text";
    tab_rel.addr_relative = inst->decalage;
    tab_rel.pointeur = inst
;
    switch(inst->typ_inst) {

   case 'R'  :
      tab_rel.mode_relocation = 2;
      break; 
	
   case 'I'  :
      tab_rel.mode_relocation = 5&6;
      break; 

   case 'J'  :
      tab_rel.mode_relocation = 4;
      break; 

  
   default : 
        tab_rel.mode_relocation = 2;
   }

}


/*Fonction pour faire le decalage */

