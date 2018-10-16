
/*Machine a etats pour l'analyse grammatical*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void machine_a_etat_gram (){
    /*definition des etats*/
    enum {INIT, DONNE, DONNE_DATA, DONNE_BSS, DEBUT,  ETIQUETTE, INSTRUCTION_TEXT,STOCKAGE};
    /*etat de l'autonome*/
    int S = INIT;
    char* c=token; /*caractere analyse courant*/

    /*definition de machine aux etats*/
    switch(S){
        case INIT:
            if(c == DIRECTIVE) S = DONNE;
            else if(c == SYMBOLE) S = DEBUT;
            break;

        case DONNE:
            if(c == .data) S = DONNE_DATA;
            else if(c == .bss) S = DONNE_BSS;
            else if(c == .setnoreorder) S = INIT; 
            break;

        case DONNE_DATA: 
            S = STOCKAGE;
            break;

        case DONE_BSS: 
            S = STOCKAGE;
            break;

        case DEBUT: 
            if(C == ':') S = ETIQUETTE;
            else S = INSTRUCTION_TEXT;
            break;

        case ETIQUETTE:
            S = STOCKAGE;
            break;

        case INSTRUCTION_TEXT: 
            S = STOCKAGE;
            break;

        case STOCKAGE: 
            /*Il faut ajoute le stockage pour chaquun des etats*/
            break;    
    }     
}
