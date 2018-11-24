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
    switch(inst->type_inst) {

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
ListeG* trouver_pseudoinstruction(Instruction* listeInstr,Dico tableau[],int dec_text, int position){
	/*Liste pour aficher tous les pseudoinstructions trouvé*/	
	
	while(listeInstr->suiv != NULL){
;
		if(inst->nom == "nop"||"lw"||"sw"||"move"||"neg"||"li"||"blt")
			return listeInstr;
	}

	return tableu_pseudoInstr;
}





/*Fonction pour remplacer un autre instruction dans la liste*/
instruction* remplacer_instr(Instruction* instr, char* nom_instr_final, int nombop_instr_final, char type_instr_final, Operande op1, Operande op2, Operande op3, ListeG* listeInstr){
	instr2->nom = nom_instr_final;
	instr2->type = instr2->type;
	instr2->type_inst = type_instr_final;
	instr2->nbop = nombop_instr_final;
	instr2->ligne = instr->ligne;
	instr2->decalage instr->decalage;
	instr2->op[0] = op1;
	instr2->op[1] = op2;
	instr2->op[2] = op3;
	

	(listeInstr->pval) = instr2; 	

}


/*fonction pour faire la lecture pseudoinstruction*/

void* lecturePseudoInstruc (Instruction* instr, ListeG* listeInstr){

	switch (instr->nom)
		case "nop"
			remplacer_instr(instr, "sll", 3, 'I'/*"sa"*/,0 ,0, 0, listeInstr);
			break;

		case "lw"
			remplacer_instr(instr, "lui", 1, "I", instr->op[0], 0, 0, listeInstr);
			*listeInstr=ajouterQueue(creerInstruction("sw", instr->categorie ,2, instr->ligne, instr->decalage , "I"/*,ListeG operande*/), *listeInstr);
			break;

		case "sw"
			remplacer_instr(instr, "lui", 1, "I", instr->op[0], 0, 0, listeInstr);
			*listeInstr=ajouterQueue(creerInstruction("sw", instr->categorie ,2, instr->ligne, instr->decalage , "I"/*,ListeG operande*/), *listeInstr);
			break;

		case "move"
			remplacer_instr(instr, "add", 3, "R", instr->op[0], instr->op[1], instr->op[2], listeInstr);
			break;

		case "neg"
			remplacer_instr(instr, "sub", 3, "R", instr->op[0], instr->op[1], instr->op[2], listeInstr);
			break;

		case "li"
			remplacer_instr(instr, "addi", 3, "I", instr->op[0], instr->op[1], instr->op[2], listeInstr);
			break;

		case "blt"
			remplacer_instr(instr, "slt", 3, "R", instr->op[0], instr->op[1], instr->op[2], listeInstr);
			*listeInstr=ajouterQueue(creerInstruction("bne", instr->categorie ,3, instr->ligne, instr->decalage , "I"/*,ListeG operande*/), *listeInstr);
			break;

}



/*Fonction pour faire le mangement des pseudo-instructions */

/*
Instruction* pseudoInstruction(Dico tableau[], File F, ListeG* Inst, int dec_text, int position){
	File G = F->suiv;
	

	if (strcmp(p->symbole,"lw")==0){
		*Inst=ajouterQueue(creerInstruction("lui", "SYMBOLE",tableau[position].operands, G->ligne, dec_text), *Inst);
		*Inst=ajouterQueue(creerInstruction("lw", "SYMBOLE",tableau[position].operands, G->ligne, dec_text), *Inst);

	}else if (strcmp(p->symbole,"sw")==0){
		*Inst=ajouterQueue(creerInstruction("lui", "SYMBOLE",tableau[position].operands, G->ligne, dec_text), *Inst);
		*Inst=ajouterQueue(creerInstruction("sw", "SYMBOLE",tableau[position].operands, G->ligne, dec_text), *Inst);

	}else if (strcmp(p->symbole,"nop")==0){
		*Inst=ajouterQueue(creerInstruction("sll", "SYMBOLE",tableau[position].operands, G->ligne, dec_text), *Inst);

	}else if (strcmp(p->symbole,"move")==0){
		*Inst=ajouterQueue(creerInstruction("add", "SYMBOLE",tableau[position].operands, G->ligne, dec_text), *Inst);

	}else if (strcmp(p->symbole,"li")==0){
		*Inst=ajouterQueue(creerInstruction("addi", "SYMBOLE",tableau[position].operands, G->ligne, dec_text), *Inst);

	}else if (strcmp(p->symbole,"blt")==0){
		*Inst=ajouterQueue(creerInstruction("slt", "SYMBOLE",tableau[position].operands, G->ligne, dec_text), *Inst);
		*Inst=ajouterQueue(creerInstruction("bne", "SYMBOLE",tableau[position].operands, G->ligne, dec_text), *Inst);

	}	
}


*/




/*Fonction pour faire le decalage */

