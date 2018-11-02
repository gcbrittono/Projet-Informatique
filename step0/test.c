#include <stdio.h>

int* tableau(){
	int tab[5];
	for (i=0;i<5;i++)
		tab[i]=i;
	return tab;
}

main(){
int tab[5];
tab=tableau();
for (int i=0;i<5;i++)
		printf("%d \n", tab[i]);
}
