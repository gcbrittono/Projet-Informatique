
/*Dictionnaire pour reconaitre les structures*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


/*Definition du variable type DicIn*/
typedef struct DicIn DicIn;
 
/*Definition de structure pour charger les donees de l'instruction dans u dictionnaire, il y a les donnes de symbole de l'instruction, type soit r,i ou j et le nombre des operands*/
struct DicIn{ 
    char* symbole;
    char type;
    int operands;
};

/*Fonction utilisé pour charger les*/
DicIn ChargeDictionnaire(){
    
    FILE* dictionnaire_instruction;
    DicIn* dictionnaire;
    int nombreInstruc;

    dictionnaire_instruction = fopen("dictionnaire_instruction.txt","r");
    if (dictionnaire_instruction == NULL) return NULL;

    nombreInstruc = fscanf(dictionnaire_instruction, "%d", nombreInstruc);    
    if(nombreInstruc != 29) return NULL;
    
    dictionnaire = calloc(nombreInstruc,sizeof(*dictionnaire));

    /*Parcourir le fichier avec les instruction a utiliser et apres les ajouter a la structure*/

    for(i = 0; i<nombreInstruc; i++){
        if ((fscanf(dictionnnaire_instruction, "%s %c %d", &(dictionnaire[i].symbole),&(dictionnaire[i].type),&(dictionnaire[i].operands))!=3)){
            free(dictionnaire);
            return NULL
        }   
    }
    fclose(dictionnaire_instruction);
    return dictionnaire;

}
