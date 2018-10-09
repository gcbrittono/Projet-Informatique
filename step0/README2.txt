# Projet Informatique Assembleur MIPS

L'organisation du fichier est détaillée ci dessous : 
.
├── tests
│   └── miam.s
|   |__ donnees_simple.s
|   |__ instructions_test.s
|   |__ Fonction_Liste.c
|   |__ Fonction_Liste.h
|   |__ Test_File.s
├── src
│   ├── main.c
│   └── lex.c
├── include
│   ├── notify.h
│   ├── lex.h
│   └── global.h
├── doc
│   └── Livrabre1_BRITTON_WRIGHT.pdf
|   |__ README.txt
├── README.txt
├── Makefile
└── Doxyfile

En ce document on peut trouver les fichiers concernant la construction d'un compilateur en code C pour le rapport entre le langage assembleur et le codage binaire entendable pour le processeur MIPS. Le projet a pour but familiariser aux étudiants par rapport aux différentes étapes concernant le processus avec quelques techniques d'embarquement utilisées en l'informatique actuelle.  

#L'installation
Pour l'installation du programme il faut ouvrir un terminal ou on a décompressé les fichiers du projet et typée make all, après l'exécution tous les fichiers nécessaires seront inclus dans la file step0.

#L'execution
L'exécution du programme se fait avec l'instruction ./as-mips FICHIER. S, après d'avoir enregistré le command make all.

#Debugging et test
Pour le debuging et test il est nécessaire d'ouvrir un terminal sur le dossier du projet et typer le command make debug, après on ajoute le commandé ./as-mips FICHIER.S.
Pour tester les fonctions sur lesw file, il faut se placer dans le répertoire test et faire les commandes suivantes : 
	gcc Fonction_Liste.c -o Fonction_Liste
	gcc Fonction_Liste.c Test_File -o Test_File
	./Test_File

#Le dossier du projet
Les fichiers contenues dans le dossier sont:
    *doc: Contient les documents de rapport et les livrables de la cour.    
    *include: Contient les fichiers concernant les libraries d'exécution.    
    *Src: Contient les fichiers et code base pour le fonctionnement du compilateur.    
    *test: Contient les fichiers concernant le debuging et le test du programme.    
    *Doxyfile: Fichier concerné aux configurations du code.    
    *Makefile: Fichier utilisé pour simplifier la compilation du code.    
    *README.txt: Fichier de rapport de l'utilisation du projet est du contenue.
 
#Les authors
Gilles Wright
Giovani Britton

