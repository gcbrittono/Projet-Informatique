#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <gram.h>
#include <lex.h>
#include <rel.h>

void registre( char* reg, registres tab[32], int ligne){
	int i=0;;
	int ok=0;
	int indice=-1;
	while (i<32 ){
		if (strcmp(reg,tab[i].mnemo)==0)
			indice=i;
		else if(strcmp(reg,tab[i].reg)==0)
			ok=1;
		i+=1;
	}
	if(indice!=-1)
		strcpy(reg,tab[indice].reg);
	else if(ok==0)
		printf("erreur ce registre n'existe pas ligne %d \n",ligne);/*gestion erreurs arreter compil?*/
}

ListeG inserer(void* e, ListeG L){
	ListeG A=calloc(1,sizeof(e)+sizeof(A->suiv));
	if (A==NULL)
		return NULL;
	(A->pval)=e;
	if (listeVide(L)){
		A->suiv=A;
		return A;
	}
	else{
		A->suiv=L->suiv;
		L->suiv=A;
	}
	return L;
}

void associerReg(ListeG Inst,registres tableau[32],int ligne){
	int nb=((Instruction*)(Inst->pval))->nbop;
	int i = 0;
	for(i; i<nb;i++){
		if(((Instruction*)(Inst->pval))->op[i].categorie==REGISTRE)
			registre(((Instruction*)(Inst->pval))->op[i].lexeme,tableau,ligne);
	}
}

void chargeRegistre(registres tab[32]){
FILE* re;

	re = fopen("src/registre.txt","r");
	if (re == NULL){
		printf("le dictionnaire de registre n'a pas été ouvert \n"); 
		return; /*gestion erreurs*/
	}
	char* reg1[10]/*=malloc(sizeof(*reg1))*/;
	char* reg2[4]/*=malloc(sizeof(*reg2))*/;
	int i;
	for (i=0;i<32;i++){
        	fscanf(re, "%s %s",reg1,reg2);
		tab[i].mnemo=strdup(reg1);
		tab[i].reg=strdup(reg2);
	}
	fclose(re);
}


/*Fonction pour créer la table de relocation à partir de l'étiquette*/
table_relocation*  symbole_find(ListeG L, Symbole* symb, int i /*entier qui indique si data ou text*/){
	table_relocation* tab_rel;
	if(i==0){
	/*if(symb->sect==DATA){*/
     		tab_rel->nom_section =strdup("DATA");
            	tab_rel->addr_relative = ((Donnee1*)(L->pval))->decalage;/*symb->decalage*/
            	tab_rel->mode_relocation = 2;
		tab_rel->pointeur = symb;		
		}
	else if (i==1){
        /*else if(symb->sect==TEXT){*/
            	tab_rel->nom_section =strdup("TEXT");
            	tab_rel->addr_relative = ((Instruction*)(L->pval))->decalage;/*symb->decalage*/
		switch(((Instruction*)(L->pval))->type_inst){
			case 'R'  :
      				tab_rel->mode_relocation = 2;
      				break; 
	
   			case 'I'  :
      				tab_rel->mode_relocation = 5&6;
      				break; 

   			case 'J'  :
      				tab_rel->mode_relocation = 4;
      				break; 

  
   			default : 
        			tab_rel->mode_relocation = 2;
		}
            	/*tab_rel.mode_relocation = 2; */ /*Je ne suis pas sur si ca marcherá dans le cas text*/
		tab_rel->pointeur = symb;
	}	
	return tab_rel;   
}


/*Fonction pour remplacer un autre instruction dans la liste*/
void remplacer_instr(ListeG listeInstr, char* nom_instr_final, int nombop_instr_final, char type_instr_final, char* ope[], etat typop[]){
	free(((Instruction*)(listeInstr->pval))->nom );
	((Instruction*)(listeInstr->pval))->nom = strdup(nom_instr_final);
	((Instruction*)(listeInstr->pval))->type_inst = type_instr_final;
	((Instruction*)(listeInstr->pval))->nbop = nombop_instr_final;
	int i=0;
	for(i;i<nombop_instr_final;i++){
		if(((Instruction*)(listeInstr->pval))->op[i].lexeme!=NULL)
			free(((Instruction*)(listeInstr->pval))->op[i].lexeme);
		((Instruction*)(listeInstr->pval))->op[i].lexeme = strdup(ope[i]);
		((Instruction*)(listeInstr->pval))->op[i].categorie=typop[i];
	}
}

/*Fonction qui remplace les pseudo instructions 
la fonction décale le pointeur de la liste lorsqu'elle ajoute une instruction*/
void pseudoInstruction( ListeG* instr){
	char* o[3];
	etat typop[3];
	if (strcmp(((Instruction*)((*instr)->pval))->nom,"lw")==0){
		o[0]=strdup(((Instruction*)((*instr)->pval))->op[0]);
		o[1]=strdup(((Instruction*)((*instr)->pval))->op[1]);
		typop[0]=((Instruction*)((*instr)->pval))->op[0].categorie;
		typop[1]=((Instruction*)((*instr)->pval))->op[1].categorie;
		*instr=inserer(creerInstruction("lw", ((Instruction*)((*instr)->pval))->type, 2, ((Instruction*)((*instr)->pval))->ligne, (((Instruction*)((*instr)->pval))->decalage)+4 , 'I'), *instr);
		((Instruction*)((*instr)->suiv->pval))->op[0].categorie=((Instruction*)((*instr)->pval))->op[0].categorie;
		((Instruction*)((*instr)->suiv->pval))->op[0].lexeme=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		((Instruction*)((*instr)->suiv->pval))->op[1].categorie=((Instruction*)((*instr)->pval))->op[1].categorie;/*remplacer par base offset une fois créé*/
		char mot[256];
		strcpy(mot,((Instruction*)((*instr)->pval))->op[1].lexeme);
		strcat(mot,"(");
		strcat(mot,((Instruction*)((*instr)->pval))->op[0].lexeme);
		strcat(mot,")");
		((Instruction*)((*instr)->suiv->pval))->op[1].lexeme=strdup(mot);
		remplacer_instr(*instr, "lui", 2, 'I',o,typop);
/*faire la relocation ici pour les deux instructions*/
	}
	else if (strcmp(((Instruction*)((*instr)->pval))->nom,"sw")==0){
		o[0]=strdup("$1");
		o[1]=strdup(((Instruction*)((*instr)->pval))->op[1].lexeme);
		typop[0]=((Instruction*)((*instr)->pval))->op[0].categorie;
		typop[1]=((Instruction*)((*instr)->pval))->op[1].categorie;
		*instr=inserer(creerInstruction("sw", ((Instruction*)((*instr)->pval))->type, 2, ((Instruction*)((*instr)->pval))->ligne, (((Instruction*)((*instr)->pval))->decalage)+4 , 'I'), *instr);
		((Instruction*)((*instr)->suiv->pval))->op[0].categorie=((Instruction*)((*instr)->pval))->op[0].categorie;
		((Instruction*)((*instr)->suiv->pval))->op[0].lexeme=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		((Instruction*)((*instr)->suiv->pval))->op[1].categorie=((Instruction*)((*instr)->pval))->op[1].categorie;/*remplacer par base offset une fois créé*/
		char mot[256];
		strcpy(mot,((Instruction*)((*instr)->pval))->op[1].lexeme);
		strcat(mot,"($1)");
		((Instruction*)((*instr)->suiv->pval))->op[1].lexeme=strdup(mot);
		remplacer_instr(*instr, "lui", 2, 'I',o, typop);
		
/*faire la relocation ici pour les deux instructions*/
	}
	else if (strcmp(((Instruction*)((*instr)->pval))->nom,"nop")==0){
		o[0]=strdup("$0");
		o[1]=strdup("$0");
		o[2]=strdup("0");
		typop[0]=REGISTRE;
		typop[1]=REGISTRE;
		typop[2]=DECIMAL;
		remplacer_instr(*instr, "sll", 3, 'I'/*"sa"*/,o, typop);
	}
	else if (strcmp(((Instruction*)((*instr)->pval))->nom,"move")==0){
		o[0]=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		o[1]=strdup(((Instruction*)((*instr)->pval))->op[1].lexeme);
		o[2]=strdup("$0");
		typop[0]=((Instruction*)((*instr)->pval))->op[0].categorie;
		typop[1]=((Instruction*)((*instr)->pval))->op[1].categorie;
		typop[2]=REGISTRE;
		remplacer_instr(*instr, "add", 3, 'R',o, typop);
	}
	else if (strcmp(((Instruction*)((*instr)->pval))->nom,"li")==0){
		o[0]=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		o[1]=strdup("$0");
		o[2]=strdup(((Instruction*)((*instr)->pval))->op[1].lexeme);
		typop[0]=((Instruction*)((*instr)->pval))->op[0].categorie;
		typop[1]=REGISTRE;
		typop[2]=((Instruction*)((*instr)->pval))->op[1].categorie;
		remplacer_instr(*instr, "addi", 3, 'I',o, typop);
	}
	else if (strcmp(((Instruction*)((*instr)->pval))->nom,"blt")==0){
		o[0]=strdup("$1");
		o[1]=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		o[2]=strdup(((Instruction*)((*instr)->pval))->op[1].lexeme);
		typop[0]=REGISTRE;
		typop[1]=((Instruction*)((*instr)->pval))->op[0].categorie;
		typop[2]=((Instruction*)((*instr)->pval))->op[1].categorie;
		*instr=inserer(creerInstruction("bne", ((Instruction*)((*instr)->pval))->type, 3, ((Instruction*)((*instr)->pval))->ligne, (((Instruction*)((*instr)->pval))->decalage)+4 , 'I'), *instr);
		((Instruction*)((*instr)->suiv->pval))->op[0].categorie=REGISTRE;
		((Instruction*)((*instr)->suiv->pval))->op[0].lexeme=strdup("$1");
		((Instruction*)((*instr)->suiv->pval))->op[1].categorie=REGISTRE;
		((Instruction*)((*instr)->suiv->pval))->op[1].lexeme=strdup("$0");
		((Instruction*)((*instr)->suiv->pval))->op[2].categorie=((Instruction*)((*instr)->pval))->op[2].categorie;
		((Instruction*)((*instr)->suiv->pval))->op[2].lexeme=strdup(((Instruction*)((*instr)->pval))->op[2].lexeme);
		remplacer_instr(*instr, "slt", 3, 'R',o, typop);
		
		
/*faire la relocation ici*/
	}
	else if (strcmp(((Instruction*)((*instr)->pval))->nom,"neg")==0){
		o[0]=strdup(((Instruction*)((*instr)->pval))->op[0].lexeme);
		o[1]=strdup("$0");
		o[2]=strdup(((Instruction*)((*instr)->pval))->op[1].lexeme);
		typop[0]=((Instruction*)((*instr)->pval))->op[0].categorie;
		typop[1]=REGISTRE;
		typop[2]=((Instruction*)((*instr)->pval))->op[1].categorie;
		remplacer_instr(*instr, "sub", 3, 'R',o, typop);
	}
}





void rel(ListeG* Instruct, ListeG Data, ListeG Etiquette, ListeG* RelocInst, ListeG* RelocData){
	registres tab[32];
	chargeRegistre(tab);

	if(listeVide(*Instruct))
		printf("la section TEXT est vide");
	else{
		ListeG A=*Instruct;
		do{
			int vrai=0;
			/*modifie les registre en chiffre*/
			associerReg(A->suiv,tab,((Instruction*)(A->suiv->pval))->ligne);
			/*insertion des pseudo instruction*/	
			if(strcmp(((Instruction*)(A->suiv->pval))->nom,"nop")==0 || (strcmp(((Instruction*)(A->suiv->pval))->nom,"lw")==0 && ((Instruction*)(A->suiv->pval))->op[1].categorie==SYMBOLE) || (strcmp(((Instruction*)(A->suiv->pval))->nom,"sw")==0 && ((Instruction*)(A->suiv->pval))->op[1].categorie==SYMBOLE) || strcmp(((Instruction*)(A->suiv->pval))->nom,"neg")==0 || (strcmp(((Instruction*)(A->suiv->pval))->nom,"blt")==0 && ((Instruction*)(A->suiv->pval))->op[2].categorie==SYMBOLE) || strcmp(((Instruction*)(A->suiv->pval))->nom,"move")==0 || strcmp(((Instruction*)(A->suiv->pval))->nom,"li")==0 )/*vérifier les conditions*/{
			if(strcmp(((Instruction*)(A->suiv->pval))->nom,"lw")==0 || strcmp(((Instruction*)(A->suiv->pval))->nom,"sw")==0 || strcmp(((Instruction*)(A->suiv->pval))->nom,"blt")==0)
				vrai=1;
			pseudoInstruction( &(A->suiv));
			if(vrai==1)
				A=A->suiv;
			}
			A=A->suiv;
		}while (A!=*Instruct);
	}
}




