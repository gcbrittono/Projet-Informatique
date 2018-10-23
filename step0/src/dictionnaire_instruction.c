
/*Dictionnaire pour reconaitre les structures*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


#define HASH_SIZE 1000

/*Definition du variable type DicIn*/
typedef struct DictionnaireIn DictionnaireIn;
 
/*Definition de structure pour charger les donees de l'instruction dans un dictionnaire, il y a les donnes de symbole de l'instruction, type soit r,i ou j et le nombre des operands*/
struct DictionnaireIn{ 
    char* symbole;
    char type;
    int operands;
};




//-----------------------------------------------------------------------------------------------------------------------------------------
// HASH
//-----------------------------------------------------------------------------------------------------------------------------------------

//Fonction qui a comment entrée la chaine des characters et comment sortie le code hash liée a cette entrée
int funHash(char* str){ // Trouver en https://stackoverflow.com/questions/7666509/hash-function-for-string
	char new[32];
	snprintf(new, 32, "%s", str);
	toLowerStr(new);
	
	int hash, i;
	int len = strlen(new);
	
	
    for(hash = i = 0; i < len; ++i)
    {
        hash += new[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    
    return (int)fabs(hash % HASH_SIZE);
}


//Fonction utilisée dans la fonction funHash pour convertir les character string en son equivalent minuscule
void toLowerStr(char *str){
	int lenght = strlen(str);
	int i;
	for(i = 0; i < lenght; i++){
		str[i] = tolower(str[i]);
	}
}


//Fonction pour insérer le symbole dans un position données dans le tableau hash
void insérer (char* str, DictionnaireIn* hashTableDictionnaireIn){
	int index = funHash(str);
	char hashTableDictionnaireIn[index] = str;

}

//Fonction pour trouver les elements dans le tableau en ayant l'index
char* rechercer_symb(int index, DictionnaireIn* hashTableDictionnaireIn){
	return hashTableDictionnaireIn[index];
}


//Fonction pour generer un tableu pour stocker les structure de type DictionnaireIn, generate hash tableau
DictionnaireIn generateHashTableDictionnaireIn(){

	FILE* dictionnaire_instruction;
	int nombreInstruc;

	dictionnaire_instruction = fopen("dictionnaire_instruction.txt","r");
	if (dictionnaire_instruction == NULL) return NULL;

	fscanf(dictionnaire_instruction, "%d", nombreInstruc);    
	if(nombreInstruc == EOF) return NULL;

	DictionnaireIn* hashTableDictionnaireIn[nombreInstruc];

	for(int i = 0; i <= nombreInstruc; i++){
		hashTable[nombreInstruc]->symbole = "EMPTY";
		hashTable[nombreInstruc]->type = 'X';
		hashTable[nombreInstruc]->operands = 0;	
	}
	fclose(dictionnaire_instruction);
	return hashTableDictionnaireIn;
} 


//-----------------------------------------------------------------------------------------------------------------------------------------


/*Fonction utilisé pour charger les*/
/*DictionnaireIn ChargeDictionnaire(){
    
    FILE* dictionnaire_instruction;
    
    int nombreInstruc;

    dictionnaire_instruction = fopen("dictionnaire_instruction.txt","r");
    if (dictionnaire_instruction == NULL) return NULL;

    nombreInstruc = fscanf(dictionnaire_instruction, "%d", nombreInstruc);    
    if(nombreInstruc != 29) return NULL;
    DictionnaireIn dictionnaire[nombreInstruc];
    //dictionnaire = calloc(nombreInstruc,sizeof(*dictionnaire));

    //Parcourir le fichier avec les instruction a utiliser et apres les ajouter a la structure

    for(i = 0; i<nombreInstruc; i++){
        if ((fscanf(dictionnnaire_instruction, "%s %c %d", &(dictionnaire[i].symbole),&(dictionnaire[i].type),&(dictionnaire[i].operands))!=3)){
            //free(dictionnaire);
            //return NULL;
        }   
    }
    fclose(dictionnaire_instruction);
    return dictionnaire;
 
}*/
