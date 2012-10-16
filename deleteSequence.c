
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include "tarea1.h"
#include "RTree.h"


int deleteMethod;
void randDelete(node *no, RTree *t){
	int j = round(drand48()*(no->size-1));
	if(no==NULL)
		fprintf(stderr,"Intento de acceso a nodo nulo");

	if(no->leaf){
		rect *r1=dupRect(no->values[j]->r);
		if(deleteMethod == 1)
			delete(r1,t,no->values[j]->child);
		else
			delete2(r1,t,no->values[j]->child);
	}
	else{
		node *n2=readNode(no->values[j]->child);
		randDelete(n2,t);
		freeNode(n2);
	}

}

int main(int argc, char *argv[]){
	srand48(time(NULL));
	int i;
	RTree *tree = readTree();

	int m = strtol(argv[1], NULL, 10);

	deleteMethod = strtol(argv[2], NULL, 10);

	printf("-----------------\n");
	printf("Número de borrados:  %d\n", m);
	for(i=0;i<m;i++)
		randDelete(tree->root,tree);

	printf("Lecturas de Archivos: %d\n", getReads());
	printf("Escrituras de Archivos: %d\n", getWrites());

	printf("-----------------\n");

	writeRTree(tree);

}

