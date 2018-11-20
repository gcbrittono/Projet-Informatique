#include <rel.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>







/*typedef enum {R_MIPS_32=2, R_MIPS_26=4, R_MIPS_HI16=5,
R_MIPS_LO16=6} mode_rel;*/

/*Fonction pour trouver un symbole*/
table_relocation  symbole_find(Symbole* symb){
    
    table_relocation tab_rel;

        if(symb->sect==0){
            tab_rel.nom_section = "data";
            tab_rel.addr_relative = symb->decalage;
            tab_rel.mode_relocation = 2;
         /*   tab_rel.pointeur = symb;*/
				
		}
        else if(symb->sect==2){
            tab_rel.nom_section = "text";
            tab_rel.addr_relative = symb->decalage;
            tab_rel.mode_relocation = 2;  /*Je ne suis pas sur si ca marcherá dans le cas text*/
		/*	tab_rel.pointeur = symb;*/	
		}

    
}

/*Fonction pour trouver un instruction*/
table_relocation  instruction_find(Instruction* inst){
    
    table_relocation tab_rel;

    tab_rel.nom_section = "text";
    tab_rel.addr_relative = inst->decalage;
    /*tab_rel.pointeur = inst*/
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


/*Fonction pour parcourir une liste et trouver un pseudoinstruction*/
ListeG* trouver_pseudoinstruction(/*Instruction* inst*/ File listeInstr,Dico tableau[],int dec_text, int position){
	/*Liste pour aficher tous les pseudoinstructions trouvé*/	
	ListeG* tableu_pseudoInstr;
	Instruction *inst=malloc(sizeof(*inst));;
	
	while(listeInstr->suiv != NULL){
		*inst = creerInstruction(listeInstr->lexeme, listeInstr->categorie,tableau[position].operands, listeInstr->ligne, dec_text);
		if(inst->nom == "nop"||"lw"||"sw"||"move"||"neg"||"li"||"blt")
			tableu_pseudoInstr = ajouterQueue(inst,tableu_pseudoInstr);
	}

	return tableu_pseudoInstr;
}






/*Fonction pour faire le decalage */

